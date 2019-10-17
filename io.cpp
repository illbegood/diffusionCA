#include "io.h"
#include "model.h"
#include "controller.h"
#include <fstream>

using namespace std;

int IO::cmd_load(vector<string>& args, Controller& c)
{
	int ret = 3;
	ifstream file;
	if (args.size() < 2)
		return NOT_ENOUGH_ARGUMENTS;
	file.open(args[1]);
	if (file.is_open()) {
		size_t x = 0, y = 0;
		double p = -1;
		file >> x >> y >> p;
		if (x && y && (p + 1)) {
			vector<unsigned> values;
			unsigned i;
			if (file) {
				while (file >> i)
					values.push_back(i);
				if (values.size() == 0) {
					values.reserve(x*y);
					ret = OK;
				}
				else if (values.size() != x*y)
					ret = INVALID_FILE_FORMAT;
				else
					ret = OK;
			}
			else
				ret = ERROR_WHILE_READING;
			c.setCA(new CA2D(x, y, p, values));
		}
		file.close();
		return ret;
	}
	return CANNOT_OPEN_FILE;
}

int IO::cmd_save(vector<string>& args, Controller& c)
{
	int ret;
	CA2D &ca = c.getCA();
	ofstream file;
	if (args.size() < 2)
		return NOT_ENOUGH_ARGUMENTS;
	file.open(args[1]);
	if (file.is_open()) {
		size_t count = 0, x = ca.getSizeX(), y = ca.getSizeY();
		vector<unsigned> v = ca.getParticles();
		file << ca.getSizeX() << " " << ca.getSizeY() << " " << ca.getPMove() << endl;
		for (size_t i = 0; i < y; ++i) {
			for (size_t j = 0; j < x; ++j) {
				if (!(file << v[i * y + j] << " ")) break;
				++count;
			}
			if (!(file << endl)) break;
		}
		ret = count == v.size() ? OK : ERROR_WHILE_WRITING;
		file.close();
		return ret;
	}
	return CANNOT_OPEN_FILE;
}

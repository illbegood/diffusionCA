#include "io.h"
#include "model.h"
#include "controller.h"
#include <fstream>

using namespace std;

int IO::load(vector<string>& args, Controller& c)
{
	ifstream file;
	if (args.size() < 2)
		return 1;
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
					fill(values.begin(), values.end(), 0);
				}
				else if (values.size() != x * y)
					return 2;
			}
		}
		file.close();
		return 0;
	}
}

int IO::save(vector<string>& args, Controller& c)
{
	void *ret = 0;
	CA2D &ca = c.getCA();
	ofstream file;
	if (args.size() < 2)
		return 1;
	file.open(args[1]);
	if (file.is_open()) {
		file << ca.getSizeX() << " " << ca.getSizeY() << " " << ca.getPMove() << endl;
		auto v = ca.getParticles();
		size_t i;
		for (i = 0; i < v.size(); ++i)
			if (!(file << i))
				break;
		if (i != v.size())
			return 3;
	}
	return 0;
}

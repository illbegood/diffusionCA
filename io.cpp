#include "io.h"
#include "model.h"
#include "controller.h"
#include <fstream>

using namespace std;

int IO::cmd_load(const vector<string>& args, Controller& c)
{
	unsigned argIndex = 1;
	int ret;
	if (args.size() <= argIndex)
		return NOT_ENOUGH_ARGUMENTS;
	ifstream file(args[argIndex++]);
	if (file.is_open()) {
		unsigned dim, totalSize = 1;
		double p;
		file >> dim;
		vector<size_t> size(dim);
		for (unsigned i = 0; i < dim; ++i) {
			file >> size[i];
			totalSize *= size[i];
		}
		file >> p;
		vector<unsigned> values;
		unsigned i;
		if (file) {
			while (file >> i)
				values.push_back(i);
			ret = values.size() == totalSize ? OK : INVALID_FILE_FORMAT;
		}
		else
			ret = ERROR_WHILE_READING;
		c.setCA(new CA(size, p, values));
		file.close();
		return ret;
	}
	return CANNOT_OPEN_FILE;
}

#include <iostream>
int IO::save(CA &ca, ofstream &file) {

	//cout << "file opened " << args[1] << endl;
	vector<size_t> size = ca.getSize();
	//cout << "got size " << args[1] << endl;
	vector<unsigned> v = ca.getParticles();
	//cout << "got values " << args[1] << endl;
	file << "Parameters: " << size.size() << " ";
	for (unsigned i = 0; i < size.size(); ++i)
		file << size[i] << " ";
	//cout << "sizes written " << args[1] << endl;
	file << ca.getPMove() << endl;
	//cout << "parameters written " << args[1] << endl;
	unsigned newLines = 0;
	size_t totalSize = ca.getTotalSize(), count;
	vector<size_t> coord(size.size());
	for (count = 0; count < totalSize; ++count) {
		for (unsigned i = 0; i < newLines; ++i)
			if (!(file << endl))
				break;
		if (!(file << v[count] << " "))
			break;
		newLines = CA::inc(coord, size);
	}
	//cout << "writing finished " << args[1] << endl;
	int ret = count == v.size() ? OK : ERROR_WHILE_WRITING;
	//file.close();
	return ret;
}

int IO::cmd_save(const vector<string>& args, Controller& c)
{
	int ret;
	unsigned argIndex = 1;
	CA *ca = c.getCA();
	if (!ca)
		return NO_CA_FOUND;
	if (args.size() < argIndex)
		return NOT_ENOUGH_ARGUMENTS;
	ofstream file(args[argIndex++]);
	if (file.is_open()) {
		ret = save(*ca, file);
		file.close();
		return ret;
	}
	return CANNOT_OPEN_FILE;
}

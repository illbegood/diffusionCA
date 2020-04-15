#ifndef IO_H
#define IO_H

#include "controller.h"
#include <vector>
#include <string>

using namespace std;

class IO {
public:
	static int cmd_load(const vector<string> &args, Controller& c);

	static int cmd_save(const vector<string> &args, Controller& c);

	static int save(CA &ca, ofstream &file);
};
#endif // !IO_H
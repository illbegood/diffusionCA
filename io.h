#ifndef IO_H
#define IO_H

#include "controller.h"
#include <vector>
#include <string>

using namespace std;

class IO {
public:
	static int load(vector<string> &args, Controller& c);

	static int save(vector<string> &args, Controller& c);
};
#endif // !IO_H
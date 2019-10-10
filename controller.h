#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "model.h"
#include "io.h"
#include <map>

typedef int(&command)(vector<string>&, Controller&);

class Controller {
public:
	void run(int argc, char **argv);

	CA2D& getCA();
private:
	CA2D* ca;
	map<string, command> commandMap = { { "load", IO::load },{ "step", /*(int(&)(vector<string>&, Controller&))*/CA2D::step },{ "save", IO::save } };
};
#endif // !CONTROLLER_H
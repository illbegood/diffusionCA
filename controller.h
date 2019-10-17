#ifndef CONTROLLER_H
#define CONTROLLER_H

class Controller;
class CA2D;
class IO;

#include "model.h"
#include "io.h"
#include <map>

enum Error {OK, NOT_ENOUGH_ARGUMENTS, INVALID_FILE_FORMAT, CANNOT_OPEN_FILE, ERROR_WHILE_READING, ERROR_WHILE_WRITING};

class Controller {
public:
	void run(int argc, char **argv);

	Controller();

	CA2D& getCA();

	void setCA(CA2D *ca);
private:
	CA2D* ca = NULL;
	typedef int(&command)(vector<string>&, Controller&);
	map<string, command> commandMap;
};
#endif
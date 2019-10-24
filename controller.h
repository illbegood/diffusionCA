#ifndef CONTROLLER_H
#define CONTROLLER_H

class Controller;
class CA2D;

#include "model.h"
#include "io.h"
#include <map>

enum Error { OK, NOT_ENOUGH_ARGUMENTS, INVALID_ARGUMENT };
enum IOError { INVALID_FILE_FORMAT, CANNOT_OPEN_FILE, ERROR_WHILE_READING, ERROR_WHILE_WRITING };
enum Mode { FOR_EACH_PARTICLE = 0, EACH_DIRECTION_EVENLY = 1 };
enum Border {NONE = 0, IMPENETRABLE = 1};

class Controller {
public:
	void run(int argc, char **argv);

	Controller();

	CA2D& getCA();

	void setCA(CA2D *ca);

	Mode getMode() {
		return mode;
	}

	void setMode(Mode m) {
		mode = m;
	}

	Border getBorder() {
		return border;
	}

	void setBorder(Border b) {
		border = b;
	}

private:
	CA2D* ca = NULL;
	Mode mode;
	Border border;
	typedef int(&command)(vector<string>&, Controller&);
	//not yet used
	map<string, command> commandMap;
	map<int, string> errorMap;
};
#endif
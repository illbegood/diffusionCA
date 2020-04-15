#ifndef CONTROLLER_H
#define CONTROLLER_H

class CA;
class Controller;

#include "model.h"
#include "io.h"
#include <map>

enum Error { OK, NOT_ENOUGH_ARGUMENTS, INVALID_ARGUMENT };
enum IOError { INVALID_FILE_FORMAT, CANNOT_OPEN_FILE, ERROR_WHILE_READING, ERROR_WHILE_WRITING };
enum CAError {NO_CA_FOUND};

class Controller {
public:
	void run(int argc, char **argv);

	Controller();

	CA* getCA();

	void setCA(CA *ca);

private:
	CA* ca = NULL;
	typedef int(&command)(const vector<string>&, Controller&);
	map<string, command> commandMap;
	//not yet used
	map<int, string> errorMap;
};
#endif
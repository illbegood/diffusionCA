#include "controller.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>

using namespace std;

Controller::Controller()
{
	commandMap = { { "load", IO::cmd_load },
	{ "step", CA::cmd_step },
	{ "save", IO::cmd_save },
	{"delta", CA::cmd_delta},
	{"blank", CA::cmd_blank},
	{"green", cmd_green },
	{"mode", CA::cmd_mode},
	{"border", CA::cmd_border} };
	//not yet used
	errorMap = { {OK, "Done!"},
	{NOT_ENOUGH_ARGUMENTS, "Error: Not enough arguments."},
	{INVALID_ARGUMENT, "Error: invalid argument"} };
}

void Controller::run(int argc, char ** argv)
{
	string line;
	while (true) {
		getline(cin, line);
		istringstream iss(line);
		vector<string> tokens(istream_iterator<string>{iss},
			istream_iterator<string>());
		auto it = commandMap.find(tokens[0]);
		if (it != commandMap.end())
			int result = it->second(tokens, *this);
		else
			cout << "Command not found!" << endl;
	}
}

CA* Controller::getCA()
{
	return ca;
}

void Controller::setCA(CA * ca)
{
	if (this->ca)
		delete this->ca;
	this->ca = ca;
}

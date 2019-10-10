#include "controller.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>

using namespace std;

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
			it->second(tokens, this);
		else
			cout << "Command not found!" << endl;
	}
}

CA2D & Controller::getCA()
{
	return *ca;
}

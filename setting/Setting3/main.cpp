/*
 * main.cpp
 *
 *  Created on: 2017. 3. 30.
 *      Author: choi.techwin
 */

#include "Parser.h"

#include <iostream>
#include <exception>
using namespace std;

int main(int argc, char** argv) {
	try {
		Parser parser;
#if 0
		parser.open("rsc/data.txt");
		int result;
		result = parser.readInt();
		cout << result << endl;
		result = parser.readInt();
		cout << result << endl;
		result = parser.readInt();
		cout << result << endl;
#else
		if(argc == 2)
		{
			parser.open(argv[1]);
			parser.readBody();
			parser.printBody();
		}

#endif
		parser.close();
	} catch (exception& e) {
		cout << e.what();
	}
	return 0;
}



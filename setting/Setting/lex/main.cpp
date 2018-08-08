/*
 * main.cpp
 *
 *  Created on: 2017. 3. 23.
 *      Author: choi.techwin
 */

#include "lex.h"
#include "parser.h"

using namespace std;

int main(int argc, char* argv[] ) {
	if(argc == 2)
	{

	Lex lex;
	    Parser parser(&lex);
	    parser.open(argv[1]);
	    //parser.read(argv[1]);

//	    VideoManager videoManager;
//	    videoManager.get(&parser, "videoManager");
//	    videoManager.write(&parser, "videoManager");
	}
	else
	{
		Lex lex;
	    Parser parser(&lex);
	    parser.open("./data.txt");
        cout << "Please specify the setting file relative path" << endl;
	}
}



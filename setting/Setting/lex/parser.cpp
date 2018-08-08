/*********************************************************************
 *                                                                              
 * Copyright 2017  Hanwha Techwin                                              
 *                                                                                                                                                                                                               
 * This software is copyrighted by, and is the sole property
 * of Hanwha Techwin. 
 * 
 * Hanwha Techwin, Co., Ltd. 
 * http://www.hanwhatechwin.co.kr 
 *********************************************************************/
/**
 * @file  parser.cpp
 * @brief This file will provide the template for C program coding
 *        standard.
 * @author : taeho07.kim
 * @date : 2017. 3. 24.
 * @version : 
 */

#include <sstream>

#include "parser.h"
#include "lex.h"
#include <fstream>

Parser::Parser(Lex* plex) :
 lexer(plex), pCurrent(0), pParent(0), fin()
{
}
Parser::~Parser()
{
	close();
}
void Parser::close()
{
	fin.close();
}

void Parser::read(string key)
{
    string fileName = key;
    try
    {
    }
    catch(...)
    {
    }
}

void Parser::open(const char* filename)
{
	fin.open(filename, std::fstream::in | std::fstream::out | std::fstream::app);
	if(fin.is_open())
	{
		int int_data;
		float fdata;
		string string_data;
		int_data = lexer->read_int(fin);
		fdata = lexer->read_float(fin);
		string_data = lexer->read_string(fin);
		cout << " " << int_data << " " << fdata << " " << string_data << endl;
	}
	else
	{
		throw 1;
		return;
	}
}

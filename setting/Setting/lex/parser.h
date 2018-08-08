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
 * @file  parser.h
 * @brief This file will provide the template for C program coding
 *        standard.
 * @author : taeho07.kim
 * @date : 2017. 3. 24.
 * @version : 
 */

#ifndef PARSER_H_
#define PARSER_H_

#include <string>

#include "lex.h"

using namespace std;


class Header;
class Structure;
class Element;


class Parser
{
private:
    Lex* lexer;
    Element *pCurrent, *pParent;
    ifstream fin;

public:
    Parser(Lex* lex);
    virtual ~Parser();

    void open(const char* filename); // TODO
    virtual void close(); // TODO
    virtual void read(string key);
};

#endif /* PARSER_H_ */

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
 * @file  tokenizer.h
 * @brief This file will provide the template for C program coding
 *        standard.
 * @author : taeho07.kim
 * @date : 2017. 3. 24.
 * @version : 
*/

#ifndef LEX_H_
#define LEX_H_

#include <ctype.h>

#include <iostream>
#include <fstream>
#include <string>
#include <exception>
#include <map>
#include <sstream>


using namespace std;

static string MAIN_KEY = "MAIN";
static string BEGIN = "{";
static string END = "}";
static string BLANKCRLF = " \t\r\n\0";

#define EXCEPTION_TYPE __COUNTER__

//std::map<int, const char*> exception_type_string_map;
//std::map<int, const char*> exception_message_string_map;

class LexException : public exception
{
private:
    int type_id;
    int message_id;
public:
    LexException(int msg_id) : type_id(EXCEPTION_TYPE)
    {
        message_id = msg_id;
    }
    virtual ~LexException()throw(){}
    virtual const char* what() const throw()
    {
        //return exception_type_string_map.find(type_id) + exception_message_string_map.find(message_id);
        return 0;
    }
    int get_typeId()
    {
        return type_id;
    }
    int get_messageId()
    {
        return message_id;
    }
};

class Lex
{
private:
        char lookahead;

private:
    bool is_blankCrLf(char c)
    {
        if(BLANKCRLF.find(c) ==  std::string::npos) // if not exists
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    bool is_delimeter(char c)
    {
	string target(1, c);
        if(is_blankCrLf(c)) return true;
	if(target == BEGIN || target == END) return true;
        return false;
    }
    bool is_period(char c)
    {
        return (c == '.')? true : false;
    }

    char read_blankcrlf(ifstream& fin)
    {
        char c = 0;
        fin.get(c);
        while(is_blankCrLf(c))
        {
            if(fin.eof()) { break; }
            fin.get(c);
        }
        return c;
    }


public:
    Lex() : lookahead('\0') { }
    virtual ~Lex() { }
    int read_int(ifstream& fin) throw(LexException)
    {
        string token;

        lookahead = this->read_blankcrlf(fin);
        //if(isdigit(lookahead) == 0) { return false; }

        while (isdigit(lookahead) != 0 && !fin.eof())
        {
            token.append(1, lookahead);
            fin.get(lookahead);
        }
	if(token.empty())
	{
		throw LexException(1);
	}
        stringstream ss;
        ss << token;
	int result;
        ss >> result;
        return result;
    }

    float read_float(ifstream& fin)
    {
        string token;
	float result = 0.;

        lookahead = this->read_blankcrlf(fin);
        if(isdigit(lookahead) == 0) { return false; }

        while ((isdigit(lookahead) != 0 || is_period(lookahead) != 0) && !fin.eof() )
        {
//            this->read_element(fin, lookahead);
            token.append(1, lookahead);
            fin.get(lookahead);
        }
        stringstream ss;
        ss << token;
        ss >> result;
        return result;
    }

    string read_string(ifstream& fin)
    {
        string token;string result;

        lookahead = this->read_blankcrlf(fin);
        while (!is_delimeter(lookahead) && !fin.eof())
        {
//            this->read_element(fin, lookahead);
            token.append(1, lookahead);
            fin.get(lookahead);
        }
        stringstream ss;
        ss << token;
        ss >> result;
        return result;
    }

    bool read_begin(ifstream& fin)
    {
	    string target(1, lookahead);
        if(target == BEGIN)
        {
            fin.get(lookahead);
            return true;
        }
        return false;
    }

    bool read_end(ifstream& fin)
    {
	    string target(1, lookahead);
        if(target == END)
        {
            fin.get(lookahead);
            return true;
        }
        return false;
    }
};

#endif /* LEX_H_ */

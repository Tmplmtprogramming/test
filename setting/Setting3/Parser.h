/*
 * Parser.h
 *
 *  Created on: 2017. 3. 30.
 *      Author: choi.techwin
 */

#ifndef PARSER_H_
#define PARSER_H_

#include <stack>

#include "Lex.h"
#include "element.h"
#include "structure.h"

class Parser {
private:
	Lex lex;
	ifstream fin;
public:
	Parser(): lex() {

	}
	virtual ~Parser() {}

	void open(string fileName) {
		this->fin.open(fileName.c_str());
		if (!this->fin.is_open())
			throw new exception();
	}
	void close() {
		fin.close();
	}

	int readInt() {
		return lex.readInt(fin);
	}

	float readFloat()
	{
	    return lex.readFloat(fin);
	}

	Element* readElement()
	{
//	    lex.readBlank(fin);
	    string name = lex.readString(fin);
	    string value = lex.readString(fin);

	    Element* data = new Element(name, value);
        return data;
	}

	Structure* parse_structure(Structure* parent, std::stack<string>& stk, unsigned int depth = 0)
	{
        bool is_value = true;
        string value;
        string name;
        Structure* structure = 0;
        while (!stk.empty())
        {
            string top_string = stk.top();
            stk.pop();
            if (top_string[0] == END)
            {
                structure = new Structure;
                structure->set_myDepth(parent->get_myDepth() + 1);
//                parse_structure(stk);
//                structure->push_element(structure);
                this->parse_structure(structure, stk, structure->get_myDepth());
                parent->push_element(structure);
            }
            else if (top_string[0] == BEGIN)
            {
                if(!stk.empty())
                {
                    string name = stk.top();
                    stk.pop();
                    if(parent)
                    {
                        parent->set_name(name);
                    }
                    else
                    {
    //                    throw exception;
                        throw "New Structure element is not created.";
                    }
                }
                break;
            }
            else
            {
                if (is_value)
                {
                    value = top_string;
                    is_value = false;
                }
                else
                {
                    name = top_string;
                    Element* new_element = NULL;
                    if(parent)
                    {
                        new_element = new Element(name, value);
                        parent->push_element(new_element);
                        new_element = NULL;
                        is_value = true;
                    }
                    else
                    {
//                        throw exception;
                        throw "Parent structure is not created!!";
                    }
                }
            }
        }
        return parent;
	}

	Structure* readBody()
	{
	    std::stack<string> stk;
	    string structure_name = lex.readString(fin);
	    stk = lex.readStructure(fin);

//	    bool is_value = true;

	    body_structure = new Structure;
	    body_structure->set_myDepth(0);
	    body_structure->set_name(structure_name);
//	    Element* new_element = NULL;

	    string value;
	    string name;

	    string top_string = stk.top();
	    if(top_string[0] == END)
	    {
	        stk.pop();
            body_structure = this->parse_structure(body_structure, stk);
	    }
	    else
	    {
//	        throw exception;
	        throw "The stack not begin with End sentence";
	    }
	    return body_structure;
	}

	void printBody()
	{
	    body_structure->print();
	}
private:
	Structure* body_structure;
};



#endif /* PARSER_H_ */

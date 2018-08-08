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

class Parser
{
private:
    Lex lex;
    ifstream fin;
    Structure* body_structure;

public:
    Parser() : lex(), fin(), body_structure(0)
    {
    }
    virtual ~Parser()
    {
        if(fin.is_open())
        {
            fin.close();
        }
        if (this->body_structure)
        {
            delete body_structure;
        }
    }

    void open(string fileName)
    {
        this->fin.open(fileName.c_str());
        if (!this->fin.is_open())
            throw new exception();
    }
    void close()
    {
        fin.close();
    }

    int readInt()
    {
        return lex.readInt(fin);
    }

    float readFloat()
    {
        return lex.readFloat(fin);
    }

    Element* readElement()
    {
        string name = lex.readString(fin);
        string value = lex.readString(fin);

        Element* data = new Element(name, value);
        return data;
    }

    Structure* parse_structure(Structure* parent, std::stack<string>& stk)
    {
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
                this->parse_structure(structure, stk);
                parent->push_element(structure);
            }
            else if (top_string[0] == BEGIN)
            {
                if (!stk.empty())
                {
                    string name = stk.top();
                    stk.pop();
                    if (parent)
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
                value = top_string;
		if(value[0] == BEGIN || value[0] == END)
		{
			throw "The order of setting is invalid!";
		}
                name = stk.top();
		if(name[0] == BEGIN || name[0] == END)
		{
			throw "The order of setting is invalid!";
		}
                stk.pop();
                if (parent)
                {
                    Element* new_element = NULL;
                    new_element = new Element(name, value);
                    new_element->set_myDepth(parent->get_myDepth()+1);
                    parent->push_element(new_element);
                    new_element = NULL;
                }
                else
                {
//                        throw exception;
                    throw "Parent structure is not created!!";
                }
            }
        }
        return parent;
    }

    Structure* readBody()
    {
        std::stack<string> stk;
        string structure_name = lex.readString(fin);
        lex.readStructure(fin, stk);

        body_structure = new Structure;
        body_structure->set_myDepth(0);
        body_structure->set_name(structure_name);

        string top_string = stk.top();
        if (top_string[0] == END)
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

    Element* find(string& key)
    {
        return this->body_structure->find(key);
    }

    void writeBody(ofstream& fos)
    {
        fos << *(this->body_structure);
    }

    bool insert_element(string key, Element* target_item)
    {
        bool result = false;
        Element* found = find(key);
        if(found)
        {
            if(found->is_structure())
            {
                result = dynamic_cast<Structure*>(found)->add_element(target_item);
            }
        }
        return result;
    }
};

#endif /* PARSER_H_ */

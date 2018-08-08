/*
 * Parser.h
 *
 *  Created on: 2017. 3. 30.
 *      Author: choi.techwin
 */

#ifndef PARSER_H_
#define PARSER_H_  "PARSER_H_"

#include "Lex.h"
#include "Serializable.h"

using namespace std;

class Parser: public Serializable
{
private:
    Lex lex;
    string path, fileName;
public:
    Parser()
    {
        this->set_structure(new Structure());
    }
    virtual ~Parser()
    {
        this->get_structure()->clear_elements();
    }

    void openIn(string path, string fileName) throw ()
    {
        this->path = path;
        this->fileName = fileName;
        this->lex.open_in(path, fileName);
        this->get_structure()->clear_elements();
    }
    // store parsed data to a class
    void read() throw ()
    {
        this->get_structure()->read(this->lex, this->fileName);
    }
    void load(Serializable& serializable, string key) throw ()
    {
        this->get_read(serializable, key);
    }
    void closeIn() throw ()
    {
        this->lex.close_in();
    }

    void openOut(string path, string fileName) throw ()
    {
        this->path = path;
        this->fileName = fileName;
        this->lex.open_out(path, fileName);
        this->get_structure()->clear_elements();
    }
    // write data to a file
    void store(Serializable& serializable, string key) throw ()
    {
        this->set_write(serializable, key);
    }
    void write() throw ()
    {
        this->get_structure()->write(this->lex);
    }
    void closeOut() throw ()
    {
        this->lex.close_out();
    }

};

#endif /* PARSER_H_ */

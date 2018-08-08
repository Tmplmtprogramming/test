/***********************************************************************************
 * Copyright(c) 2017 by Samsung Techwin, Inc.
 *
 * This software is copyrighted by, and is the sole property of Samsung Techwin.
 * All rights, title, ownership, or other interests in the software remain the
 * property of Samsung Techwin. This software may only be used in accordance with
 * the corresponding license agreement. Any unauthorized use, duplication,
 * transmission, distribution, or disclosure of this software is expressly
 * forbidden.
 *
 * This Copyright notice may not be removed or modified without prior written
 * consent of Samsung Techwin.
 *
 * Samsung Techwin reserves the right to modify this software without notice.
 *
 * Samsung Techwin, Inc.
 * KOREA
 * http://www.samsungtechwin.co.kr
 *********************************************************************************/

/**
 * @file Parser.h
 * @brief 
 * @author: 
 * @date: 
 * @version: 
 */

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


/**
 *
 * @class 
 * @brief 
 */
class Parser: public Serializable
{
private:
    Lex lex;
    string path, fileName;
public:

/**
 *
 */
    Parser()
    {
        this->set_structure(new Structure());
    }

/**
 *
 * @return 
 */
    virtual ~Parser()
    {
        this->get_structure()->clear_elements();
    }


/**
 *
 * @param path 
 * @param fileName 
 */
    void openIn(string path, string fileName) throw ()
    {
        this->path = path;
        this->fileName = fileName;
        this->lex.open_in(path, fileName);
        this->get_structure()->clear_elements();
    }
    // store parsed data to a class

/**
 *
 */
    void read() throw ()
    {
        this->get_structure()->read(this->lex, this->fileName);
    }

/**
 *
 * @param serializable 
 * @param key 
 */
    void load(Serializable& serializable, string key) throw ()
    {
        this->get_read(serializable, key);
    }

/**
 *
 */
    void closeIn() throw ()
    {
        this->lex.close_in();
    }


/**
 *
 * @param path 
 * @param fileName 
 */
    void openOut(string path, string fileName) throw ()
    {
        this->path = path;
        this->fileName = fileName;
        this->lex.open_out(path, fileName);
        this->get_structure()->clear_elements();
    }
    // write data to a file

/**
 *
 * @param serializable 
 * @param key 
 */
    void store(Serializable& serializable, string key) throw ()
    {
        this->set_write(serializable, key);
    }

/**
 *
 */
    void write() throw ()
    {
        this->get_structure()->write(this->lex);
    }

/**
 *
 */
    void closeOut() throw ()
    {
        this->lex.close_out();
    }

};

#endif /* PARSER_H_ */

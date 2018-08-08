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
 * @file Array.cpp
 * @brief 
 * @author: 
 * @date: 
 * @version: 
 */

/*
 * Array.cpp
 *
 *  Created on: 2017. 4. 6.
 *      Author: choi.sungwoon
 */

#include "Array.h"
#include "VContainer.h"
#include <sstream>


/**
 *
 */
Array::Array() :
        Structure()
{
}


/**
 *
 */
Array::~Array()
{
}


/**
 *
 * @return 
 */
ENodeType Array::get_type()
{
    return eArray;
}


/**
 *
 * @param lex 
 * @param key 
 */
void Array::read(Lex& lex, const string& key) throw ()
{
    lex.read_indexEnd();
    lex.read_begin();

    this->set_key(key);
    gMessage.show("Array", "read", key);
    // read while(not end);
    int index = 0;
    while (lex.read_end().empty() && !lex.eof())
    {
        // generate new element
        Element* pElement = 0;
        if (!lex.read_begin().empty())
        {
            pElement = new Structure();
        }
        else if (!lex.read_indexBegin().empty())
        {
            pElement = new Array();
        }
        else
        {
            pElement = new Element();
        }
        // read element data
        stringstream ss;
        ss << index;
        string Indexkey;
        ss >> Indexkey;
        pElement->read(lex, Indexkey);
        index++;
        // add element
        this->add_element(pElement);
    }
}


/**
 *
 * @param lex 
 */
void Array::write(Lex& lex) throw ()
{
    for (size_t i = 0; i < elements->size(); i++)
    {
        Element *pElement = elements->get_element(i);
        if (pElement->get_type() == eStructure)
        {
            lex.write_tab();
            lex.write_begin();
            pElement->write(lex);
            lex.write_end();
        }
        else if (pElement->get_type() == eArray)
        {
            lex.write_indexBegin();
            lex.write_indexEnd();
            lex.write_begin();
            pElement->write(lex);
            lex.write_end();
        }
        else
        {
            lex.write_tab();
            lex.write_value(pElement->get_value());
        }
    }
}


/**
 *
 * @return 
 */
bool Array::prepare_elementStorage()
{
    this->elements = new VContainer();
    return true;
}

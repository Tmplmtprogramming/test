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
 * @file Structure.cpp
 * @brief 
 * @author: 
 * @date: 
 * @version: 
 */

/*
 * Structure.cpp
 *
 *  Created on: 2017. 4. 6.
 *      Author: choi.sungwoon
 */
#include "Exception.h"
#include "Structure.h"
#include "Array.h"
#include "MContainer.h"


/**
 *
 */
Structure::Structure() : elements(0)
{
    this->prepare_elementStorage();
}

/**
 *
 */
Structure::~Structure()
{
    if(elements)
    {
        if(!elements->empty())
        {
            elements->clear();
        }
    }
}


/**
 *
 * @return 
 */
ENodeType Structure::get_type()
{
    return eStructure;
}


/**
 *
 */
void Structure::clear_elements()
{
    for (size_t i = 0; i < elements->size(); i++)
    {
        Element *pElement = elements->get_element(i);
        if (pElement->get_type() == eStructure)
        {
            Structure* pStructure = (Structure*) pElement;
            pStructure->clear_elements();
        }
    }
    this->elements->clear();
}

/**
 *
 * @param key 
 * @return 
 */
Element *Structure::get_element(const string& key) throw ()
{
    Element* pElement = this->elements->find(key);
    return pElement;
}

/**
 *
 * @param pElement 
 */
void Structure::add_element(Element *pElement) throw ()
{
    this->elements->insert(pElement->get_key(), pElement);
}


/**
 *
 * @param lex 
 * @param key 
 */
void Structure::read(Lex& lex, const string& key) throw ()
{
    this->set_key(key);
    // read while(not end);
    while (lex.read_end().empty() && !lex.eof())
    {
        // read key
        string elementdKey = lex.read_string();
        gMessage.show("Structure", "read", key);
        if (elementdKey.empty())
            throw Exception(STRUCTURE_H_, "Structure::read", key);
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
        pElement->read(lex, elementdKey);
        // add element
        this->add_element(pElement);
    }
}

/**
 *
 * @param lex 
 */
void Structure::write(Lex& lex) throw ()
{
    for (size_t i = 0; i < elements->size(); i++)
    {
        Element *pElement = elements->get_element(i);
        lex.write_key(pElement->get_key());
        if (pElement->get_type() == eStructure)
        {
            lex.write_space();
            lex.write_begin();
            pElement->write(lex);
            lex.write_end();
        }
        else if (pElement->get_type() == eArray)
        {
            lex.write_indexBegin();
            lex.write_indexEnd();
            lex.write_space();
            lex.write_begin();
            pElement->write(lex);
            lex.write_end();
        }
        else
        {
            lex.write_value(pElement->get_value());

        }
    }
}


/**
 *
 * @return 
 */
bool Structure::prepare_elementStorage()
{
    elements = new MContainer();
    return true;
}

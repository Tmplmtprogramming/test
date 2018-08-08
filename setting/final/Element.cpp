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
 * @file Element.cpp
 * @brief 
 * @author: 
 * @date: 
 * @version: 
 */

/*
 * Element.cpp
 *
 *  Created on: 2017. 4. 6.
 *      Author: choi.sungwoon
 */
#include "Element.h"


/**
 *
 */
Element::Element()
{
}

/**
 *
 */
Element::~Element()
{
}


/**
 *
 * @return 
 */
ENodeType Element::get_type()
{
    return eElement;
}


/**
 *
 * @return 
 */
const string& Element::get_key() const
{
    return key;
}

/**
 *
 * @param key 
 */
void Element::set_key(const string& key)
{
    this->key = key;
}


/**
 *
 * @return 
 */
const string& Element::get_value() const
{
    return value;
}

/**
 *
 * @param value 
 */
void Element::set_value(const string& value)
{
    this->value = value;
}


/**
 *
 * @param lex 
 * @param key 
 */
void Element::read(Lex& lex, const string& key) throw ()
{
    this->set_key(key);
    string value = lex.read_string();
    if (value.empty())
        throw Exception(ELEMENT_H_, "read-value", key);
    gMessage.show("Element", "read", key, value);
    this->set_value(value);
}

/**
 *
 * @param lex 
 */
void Element::write(Lex& lex) throw ()
{
    lex.write_key(this->key);
    lex.write_value(this->value);
}


/*
 * Element.cpp
 *
 *  Created on: 2017. 4. 6.
 *      Author: choi.sungwoon
 */
#include "Element.h"

Element::Element()
{
}
Element::~Element()
{
}

ENodeType Element::get_type()
{
    return eElement;
}

const string& Element::get_key() const
{
    return key;
}
void Element::set_key(const string& key)
{
    this->key = key;
}

const string& Element::get_value() const
{
    return value;
}
void Element::set_value(const string& value)
{
    this->value = value;
}

void Element::read(Lex& lex, const string& key) throw ()
{
    this->set_key(key);
    string value = lex.read_string();
    if (value.empty())
        throw Exception(ELEMENT_H_, "read-value", key);
    gMessage.show("Element", "read", key, value);
    this->set_value(value);
}
void Element::write(Lex& lex) throw ()
{
    lex.write_key(this->key);
    lex.write_value(this->value);
}


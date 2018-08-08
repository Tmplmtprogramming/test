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
 * @file Serializable.cpp
 * @brief 
 * @author: 
 * @date: 
 * @version: 
 */

/*
 * Serializable.cpp
 *
 *  Created on: 2017. 4. 6.
 *      Author: choi.sungwoon
 */

#include "Serializable.h"
#include "Array.h"
#include <iostream>
using namespace std;


/**
 *
 */
Serializable::Serializable()
{
    pStructure = 0;
}

/**
 *
 */
Serializable::~Serializable()
{
}


/**
 *
 * @param value 
 * @return 
 */
int Serializable::get_int(const string& value)
{
    int result;
    stringstream ss;
    ss << value;
    ss >> result;
    return result;
}


/**
 *
 * @param value 
 * @return 
 */
float Serializable::get_float(const string& value)
{
    float result;
    stringstream ss;
    ss << value;
    ss >> result;
    return result;
}


/**
 *
 * @param value 
 * @return 
 */
inline string Serializable::get_string(const int value)
{
    string result;
    stringstream ss;
    ss << value;
    ss >> result;
    return result;
}


/**
 *
 * @param value 
 * @return 
 */
inline string Serializable::get_string(const float value)
{
    string result;
    stringstream ss;
    ss << value;
    ss >> result;
    return result;
}


/**
 *
 * @return 
 */
Structure* Serializable::get_structure()
{
    return this->pStructure;
}

/**
 *
 * @param pStructure 
 */
void Serializable::set_structure(Structure* pStructure)
{
    this->pStructure = pStructure;
}

// get Serializable data from Structures

/**
 *
 * @param result 
 * @param key 
 */
void Serializable::get_read(int& result, const string& key) throw ()
{
    Element *pElement = this->pStructure->get_element(key);
    if (pElement == NULL)
    {
        gMessage.show(SERIALIZABLE_H_, "getInt-not found", key);
        return;
    }
    gMessage.show("Serializable", "getInt", key);
    string token = pElement->get_value();
    result = this->get_int(token);
}

/**
 *
 * @param result 
 * @param key 
 */
void Serializable::get_read(float& result, const string& key) throw ()
{
    Element *pElement = this->pStructure->get_element(key);
    if (pElement == NULL)
    {
        gMessage.show(SERIALIZABLE_H_, "getFloat-not found", key);
        return;
    }
    gMessage.show("Serializable", "getFloat", key);
    string token = pElement->get_value();
    result = this->get_float(token);
}

/**
 *
 * @param result 
 * @param key 
 */
void Serializable::get_read(string& result, const string& key) throw ()
{
    Element *pElement = this->pStructure->get_element(key);
    if (pElement == NULL)
    {
        gMessage.show(SERIALIZABLE_H_, "getString-not found", key);
        return;
    }
    gMessage.show("Serializable", "getString", key);
    result = pElement->get_value();
}


/**
 *
 * @param serializable 
 * @param key 
 */
void Serializable::get_read(Serializable& serializable, const string& key) throw ()
{
    // find a child structure named key
    Structure *pElement = (Structure*) this->pStructure->get_element(key);
    if (pElement == NULL)
    {
        throw Exception(SERIALIZABLE_H_, "getValueStructure-not found", key);
    }
    gMessage.show("Serializable", "pStructure", key);
    // associate the serializable with a found structure
    serializable.set_structure(pElement);
    // fill each field
    serializable.read();
}


/**
 *
 * @param result 
 * @param length 
 * @param key 
 */
void Serializable::get_read(int result[], int length, const string& key) throw ()
{
    Array *pArray = (Array*) this->pStructure->get_element(key);
    if (pArray == NULL)
    {
        throw Exception(SERIALIZABLE_H_, "getArray-not found", key);
    }
    gMessage.show("Serializable", "getArrayInt", pArray->get_key());

    for (int i = 0; i < length; i++)
    {
        string indexKey = this->get_string(i);
        Element *pElement = pArray->get_element(indexKey);
        if (pElement == NULL)
        {
            throw Exception(SERIALIZABLE_H_, "getArrayElement-not found", key, indexKey);
        }
        if (pElement->get_type() == eElement)
        {
            result[i] = this->get_int(pElement->get_value());
        }
    }
}

/**
 *
 * @param result 
 * @param length 
 * @param key 
 */
void Serializable::get_read(float result[], int length, const string& key) throw ()
{
    Array *pArray = (Array*) this->pStructure->get_element(key);
    if (pArray == NULL)
    {
        throw Exception(SERIALIZABLE_H_, "getArray-not found", key);
    }
    gMessage.show("Serializable", "getArrayFloat", pArray->get_key());

    for (int i = 0; i < length; i++)
    {
        string indexKey = this->get_string(i);
        Element *pElement = pArray->get_element(indexKey);
        if (pElement == NULL)
        {
            throw Exception(SERIALIZABLE_H_, "getArrayElement-not found", key, indexKey);
        }
        if (pElement->get_type() == eElement)
        {
            result[i] = this->get_float(pElement->get_value());
        }
    }
}

/**
 *
 * @param result 
 * @param length 
 * @param key 
 */
void Serializable::get_read(string result[], int length, const string& key) throw ()
{
    Array *pArray = (Array*) this->pStructure->get_element(key);
    if (pArray == NULL)
    {
        throw Exception(SERIALIZABLE_H_, "getArray-not found", key);
    }
    gMessage.show("Serializable", "getArrayString", pArray->get_key());

    for (int i = 0; i < length; i++)
    {
        string indexKey = this->get_string(i);
        Element *pElement = pArray->get_element(indexKey);
        if (pElement == NULL)
        {
            throw Exception(SERIALIZABLE_H_, "getArrayElement-not found", key, indexKey);
        }
        if (pElement->get_type() == eElement)
        {
            result[i] = pElement->get_value();
        }
    }
}


/**
 *
 * @param key 
 * @return 
 */
Structure* Serializable::prepare_structure(const string& key)
{
    Structure* pElement = (Structure*) this->pStructure->get_element(key);
    if (pElement == NULL)
    {
        pElement = new Structure();
        pElement->set_key(key);
        this->pStructure->add_element(pElement);
    }
    return pElement;
}

/**
 *
 * @param key 
 * @return 
 */
Array* Serializable::prepare_array(const string& key)
{
    Array* pElement = (Array*) this->pStructure->get_element(key);
    if (pElement == NULL)
    {
        pElement = new Array();
        pElement->set_key(key);
        this->pStructure->add_element(pElement);
    }
    return pElement;
}

/**
 *
 * @param key 
 * @return 
 */
Element* Serializable::prepare_element(const string& key)
{
    Element* pElement = this->pStructure->get_element(key);
    if (pElement == NULL)
    {
        pElement = new Element();
        pElement->set_key(key);
        this->pStructure->add_element(pElement);
    }
    return pElement;
}

// load data to child serializable structure

/**
 *
 * @param serializable 
 * @param key 
 */
void Serializable::set_write(Serializable& serializable, const string& key) throw ()
{
    Structure* pElement = (Structure*) this->prepare_structure(key);
    gMessage.show("Serializable", "setStructure", key);

    // write serializable data to the associated structure
    serializable.set_structure(pElement);
    serializable.write();
}

/**
 *
 * @param value 
 * @param key 
 */
void Serializable::set_write(int& value, const string& key) throw ()
{
    Element* pElement = this->prepare_element(key);
    string result = this->get_string(value);
    pElement->set_value(result);
    gMessage.show("Serializable", "setInt", key, result);
}

/**
 *
 * @param value 
 * @param key 
 */
void Serializable::set_write(float& value, const string& key) throw ()
{
    Element* pElement = this->prepare_element(key);
    string result = this->get_string(value);
    pElement->set_value(result);
    gMessage.show("Serializable", "setFloat", key, result);
}

/**
 *
 * @param result 
 * @param key 
 */
void Serializable::set_write(string& result, const string& key) throw ()
{
    Element* pElement = this->prepare_element(key);
    pElement->set_value(result);
    gMessage.show("Serializable", "setString", key, result);
}


/**
 *
 * @param value 
 * @param length 
 * @param key 
 */
void Serializable::set_write(int value[], int length, const string& key) throw ()
{
    Array* pArray = this->prepare_array(key);
    gMessage.show("Serializable", "setIntArray", key);
    for (int i = 0; i < length; i++)
    {
        string elementKey = this->get_string(i);
        Element* pElement = pArray->get_element(elementKey);
        if (pElement == NULL)
        {
            pElement = new Element();
            pElement->set_key(elementKey);
            pArray->add_element(pElement);
        }
        string result = this->get_string(value[i]);
        pElement->set_value(result);
        cout << "  " << "   setArrayElement" << "(" << elementKey << ")" << result << endl;
    }
}

/**
 *
 * @param value 
 * @param length 
 * @param key 
 */
void Serializable::set_write(float value[], int length, const string& key) throw ()
{
    Array* pArray = this->prepare_array(key);
    gMessage.show("Serializable", "setIntArray", key);
    for (int i = 0; i < length; i++)
    {
        string elementKey = this->get_string(i);
        Element* pElement = pArray->get_element(elementKey);
        if (pElement == NULL)
        {
            pElement = new Element();
            pElement->set_key(elementKey);
            pArray->add_element(pElement);
        }
        string result = this->get_string(value[i]);
        pElement->set_value(result);
        cout << "  " << "   setArrayElement" << "(" << elementKey << ")" << result << endl;
    }
}

/**
 *
 * @param value 
 * @param length 
 * @param key 
 */
void Serializable::set_write(string value[], int length, const string& key) throw ()
{
    Array* pArray = this->prepare_array(key);
    gMessage.show("Serializable", "setIntArray", key);
    for (int i = 0; i < length; i++)
    {
        string elementKey = this->get_string(i);
        Element* pElement = pArray->get_element(elementKey);
        if (pElement == NULL)
        {
            pElement = new Element();
            pElement->set_key(elementKey);
            pArray->add_element(pElement);
        }
        pElement->set_value(value[i]);
        cout << "  " << "   setArrayElement" << "(" << elementKey << ")" << value[i] << endl;
    }
}

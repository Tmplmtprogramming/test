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
 * @file Serializable.h
 * @brief 
 * @author: 
 * @date: 
 * @version: 
 */

/*
 * Serializable.h
 *
 *  Created on: 2017. 4. 1.
 *      Author: choi.sungwoon
 */

#ifndef SERIALIZABLE_H_
#define SERIALIZABLE_H_ "SERIALIZABLE_H_"

#include "Exception.h"
#include "Array.h"
#include <string>
#include <sstream>
using namespace std;


/**
 *
 * @class 
 * @brief 
 */
class Serializable
{
private:
    Structure *pStructure;

    int get_int(const string& value);
    float get_float(const string& value);

/**
 *
 * @param value 
 * @return 
 */
    template<typename T>
    T get(const string& value)
    {
        T result;
        stringstream ss;
        ss << value;
        ss >> result;
        return result;
    }
    inline string get_string(const int value);
    inline string get_string(const float value);

/**
 *
 * @param value 
 * @return 
 */
    template<typename T>
    inline string get_string(const T& value)
    {
        string result;
        stringstream ss;
        ss << value;
        ss >> result;
        return result;
    }

    Element* prepare_element(const string& key);
    Structure* prepare_structure(const string& key);
    Array* prepare_array(const string& key);

public:
    Serializable();
    virtual ~Serializable();

    Structure* get_structure();
    void set_structure(Structure* pStructure);
    // get Serializable data from Structures
    virtual void read() = 0;

    void get_read(Serializable& serializable, const string& key) throw ();
    void get_read(int& result, const string& key) throw ();
    void get_read(float& result, const string& key) throw ();
    void get_read(string& result, const string& key) throw ();
    void get_read(int result[], int length, const string& key) throw ();
    void get_read(float result[], int length, const string& key) throw ();
    void get_read(string result[], int length, const string& key) throw ();

/**
 *
 * @param serializable 
 * @param length 
 * @param key 
 */
    template<typename T>
    void get_read(T serializable[], int length, const string& key) throw ()
    {
        Array *pArray = (Array*) this->pStructure->get_element(key);
        if (pArray == NULL)
        {
            throw Exception(SERIALIZABLE_H_, "getArray-not found", key);
        }
        gMessage.show("Serializable", "getArrayStructure", pArray->get_key());

        for (int index = 0; index < length; ++index)
        {
            string indexKey = this->get_string(index);
            Element *pElement = pArray->get_element(indexKey);
            if (pElement == NULL)
            {
                throw Exception(SERIALIZABLE_H_, "getArrayElement-not found", key, indexKey);
            }
            if (pElement->get_type() == eStructure)
            {
                gMessage.show("  Structure::Array", "index = ", indexKey);
                // associate the serializable with a found structure
                serializable[index].set_structure((Structure*) pElement);
                // fill each field
                serializable[index].read();
            }
        }
    }

    // create Structures from Serializable data
    virtual void write() = 0;
    // load data to child serializable structure
    void set_write(Serializable& serializable, const string& key) throw ();
    void set_write(int& value, const string& key) throw ();
    void set_write(float& value, const string& key) throw ();
    void set_write(string& value, const string& key) throw ();
    void set_write(int value[], int length, const string& key) throw ();
    void set_write(float value[], int length, const string& key) throw ();
    void set_write(string value[], int length, const string& key) throw ();

/**
 *
 * @param serializable 
 * @param length 
 * @param key 
 */
    template<typename T>
    void set_write(T serializable[], int length, const string& key) throw ()
    {
        Array* pArray = this->prepare_array(key);
        for (int index = 0; index < length; ++index)
        {
            string elementKey = this->get_string(index);
            gMessage.show("Serializable", "setStructureArray", key, elementKey);
            Structure* pElement = (Structure *) pArray->get_element(elementKey);
            if (pElement == NULL)
            {
                pElement = new Structure();
                pElement->set_key(elementKey);
                pArray->add_element(pElement);
                serializable[index].set_structure(pElement);
            }
            serializable[index].write();
        }
    }
};
#endif /* SERIALIZABLE_H_ */

/*********************************************************************
 *                                                                              
 * Copyright 2017  Hanwha Techwin                                              
 *                                                                                                                                                                                                               
 * This software is copyrighted by, and is the sole property
 * of Hanwha Techwin. 
 * 
 * Hanwha Techwin, Co., Ltd. 
 * http://www.hanwhatechwin.co.kr 
 *********************************************************************/
/**
 * @file  element.h
 * @brief This file will provide the template for C program coding
 *        standard.
 * @author : taeho07.kim
 * @date : 2017. 4. 3.
 * @version : 
 */

#ifndef SRC_ELEMENT_H_
#define SRC_ELEMENT_H_

#include <iostream>
#include <string>

using namespace std;

class Element
{
public:
    Element(string name, string value);
    virtual ~Element();

    string get_name() const
    {
        return name;
    }

    void set_name(string name)
    {
        this->name = name;
        return;
    }

    string get_value() const
    {
        return value;
    }

    void set_value(string value)
    {
        this->value = value;
        return;
    }

    Element& operator= (const Element& rhs)
    {
        if(this == &rhs)
        {
            return *this;
        }
        this->set_name(rhs.get_name());
        this->set_value(rhs.get_value());

        return *this;
    }
    virtual bool is_structure()
    {
        return false;
    }

    virtual void print()
    {
        std::cout << "(" << name << "," << value << ")" <<std::endl;
    }

    Element();

protected:
    string name;
private:
    string value;
};

#endif /* SRC_ELEMENT_H_ */

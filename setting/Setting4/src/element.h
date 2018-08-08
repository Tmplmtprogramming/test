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

    Element& operator=(const Element& rhs)
    {
        if (this == &rhs)
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
        std::cout << "(" << name << "," << value << ")" << std::endl;
    }
    virtual Element* find(string& key)
    {
        cout << "Element Name : " << this->name << std::endl;
        Element* ret = 0;
        if (key == this->name)
        {
            ret = this;
        }
        return ret;
    }

    Element();

    virtual string serialize() const
    {
        string ss(name + " " + value);
        return ss;
    }

    unsigned int get_myDepth() const
    {
        return my_depth;
    }

    virtual void set_myDepth(unsigned int myDepth)
    {
        my_depth = myDepth;
    }


    friend ostream& operator<<(ostream& os, const Element& element);
    friend ostream& operator<<(ostream& os, const Element* element);

protected:
    string name;
    unsigned int my_depth;
private:
    string value;
};

inline ostream& operator<<(ostream& os, const Element& element)
{
    os << element.serialize();
    return os;
}

inline ostream& operator<<(ostream& os, const Element* element)
{
    os << element->serialize();
    return os;
}


#endif /* SRC_ELEMENT_H_ */

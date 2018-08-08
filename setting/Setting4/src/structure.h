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
 * @file  structure.h
 * @brief This file will provide the template for C program coding
 *        standard.
 * @author : taeho07.kim
 * @date : 2017. 4. 3.
 * @version : 
 */

#ifndef SRC_STRUCTURE_H_
#define SRC_STRUCTURE_H_

#define USE_VECTOR 0
#if USE_VECTOR
#include <vector>
#else
#include <map>
#endif
#include <sstream>

#include "element.h"

class Structure: public Element
{
public:
    Structure();
    virtual ~Structure();

    virtual bool is_structure()
    {
        return true;
    }
    virtual void set_myDepth(unsigned int myDepth)
    {
        my_depth = myDepth;
#if USE_VECTOR
        for (std::vector<Element*>::iterator itr = elements.begin(); itr != elements.end(); ++itr)
        {
            (*itr)->set_myDepth(my_depth+1);
        }
#else
        std::map<string, Element*>::iterator itr;
        for(itr = this->elements.begin(); itr != this->elements.end(); ++itr)
        {
            itr->second->set_myDepth(my_depth+1);
        }
#endif
    }

    size_t push_element(Element* element)
    {
        element->set_myDepth(this->my_depth+1);
#if USE_VECTOR
        this->elements.push_back(element);
#else
        this->elements.insert(std::make_pair(element->get_name(), element));
#endif
        return elements.size();
    }

    virtual void print()
    {
        for (size_t j = 0; j < this->get_myDepth(); ++j)
        {
            cout << " ";
        }
        cout << this->name << endl;
        for (size_t j = 0; j < this->get_myDepth(); ++j)
        {
            cout << " ";
        }
        cout << "{" << std::endl;
#if USE_VECTOR
        for (size_t i = 0; i < elements.size(); ++i)
        {
            for (size_t j = 0; j < this->get_myDepth(); ++j)
            {
                cout << " ";
            }
            if (elements[i]->is_structure())
            {
                elements[i]->print();
            }
            else
            {
                std::cout << " ";
                elements[i]->print();
            }
        }
#else
        for (std::map<string, Element*>::iterator itr = elements.begin(); itr != elements.end(); ++itr)
        {
            for (size_t j = 0; j < this->get_myDepth(); ++j)
            {
                cout << " ";
            }
            if (itr->second->is_structure())
            {
                itr->second->print();
            }
            else
            {
                std::cout << " ";
                itr->second->print();
            }
        }
#endif
        for (size_t j = 0; j < this->get_myDepth(); ++j)
        {
            cout << " ";
        }
        std::cout << "}" << std::endl;
    }

    virtual Element* find(string& key)
    {
        cout << "Element Name : " << this->name << std::endl;
        Element* ret = 0;
        if(key == this->name)
        {
            return static_cast<Element*>(const_cast<Structure*>(this));
        }
#if USE_VECTOR
        for (std::vector<Element*>::iterator itr = elements.begin(); itr != elements.end(); ++itr)
        {
            ret = (*itr)->find(key);
            if (ret != NULL)
            {
                break;
            }
        }
#else
        std::map<string, Element*>::const_iterator itr = elements.find(key);
        if (itr != elements.end())
        {
            if (key == itr->second->get_name())
            {
                ret = itr->second;
            }
            else
            {
                throw "Invalid situation!! map's key and element's name is different!!";
            }
        }
        else
        {
            for (itr = elements.begin(); itr != elements.end(); ++itr)
            {
                ret = itr->second->find(key);
                if (ret != 0)
                {
                    break;
                }
            }
        }
#endif
        return ret;
    }
    virtual string serialize() const
    {
        stringstream ss;
        ss << this->name << std::endl;
        ss << "{" << std::endl;
#if USE_VECTOR
        std::vector<Element*>::const_iterator itr;
        for(itr = this->elements.begin(); itr != this->elements.end(); ++itr)
        {
            ss << (*itr)->serialize() << std::endl;
        }
#else
        std::map<string, Element*>::const_iterator itr;
        for(itr = this->elements.begin(); itr != this->elements.end(); ++itr)
        {
            ss << itr->second->serialize() << std::endl;
        }
#endif
        ss << "}";
        return ss.str();
    }

    bool add_element(Element* item)
    {
        bool duplicated = false;
#if USE_VECTOR
        std::vector<Element*>::const_iterator itr;
        for(itr = this->elements.begin(); itr != this->elements.end(); ++itr)
        {
            if(item == (*itr) || item->get_name() == (*itr)->get_name())
            {
                duplicated = true;
                break;
            }
        }
        if(!duplicated)
        {
            this->elements.push_back(item);
        }
        return !duplicated;
#else
        std::map<string, Element*>::const_iterator itr;
        for(itr = this->elements.begin(); itr != this->elements.end(); ++itr)
        {
            if(item == itr->second || item->get_name() == itr->second->get_name())
            {
                duplicated = true;
                break;
            }
        }
        if(!duplicated)
        {
            this->elements.insert(std::make_pair(item->get_name(), item));
        }
        return !duplicated;
#endif
    }

    friend ostream& operator<<(ostream& os, const Structure& structure);
    friend ostream& operator<<(ostream& os, const Structure* structure);

private:
#if USE_VECTOR
    std::vector<Element*> elements;
#else
    std::map<string, Element*> elements;
#endif
};

inline ostream& operator<<(ostream& os, const Structure& structure)
{
    os << structure.serialize();
    return os;
}

inline ostream& operator<<(ostream& os, const Structure* structure)
{
    os << structure->serialize();
    return os;
}

#endif /* SRC_STRUCTURE_H_ */

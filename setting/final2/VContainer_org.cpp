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
 * @file  VContainer.cpp
 * @brief This file will provide the template for C program coding
 *        standard.
 * @author : taeho07.kim
 * @date : 2017. 4. 12.
 * @version : 
 */

#include "VContainer.h"

VContainer::VContainer() : Container(), elements()
{
    // TODO Auto-generated constructor stub

}

VContainer::~VContainer()
{
    // TODO Auto-generated destructor stub
    clear();
}

size_t VContainer::size() const
{
    return elements.size();
}

void VContainer::clear()
{
    size_t i = 0;
    for(i = 0; i < elements.size(); ++i)
    {
        delete elements.at(i);
    }
    elements.clear();
}

Element* VContainer::get_element(const int index) throw ()
{
    Element* result = 0;
    if(index >= 0 && (size_t)index < elements.size())
    {
        result = elements.at(index);
    }
    else
    {
        throw Exception("Map::", "get", "indexOutOfBounds", index);
    }
    return result;
}

Element* VContainer::find(const string& key) throw ()
{
    size_t i = 0;
    Element* ret = 0;
    for(i = 0; i < elements.size(); ++i)
    {
        ret = elements.at(i);
        if(ret->get_key() == key)
        {
            break;
        }
    }
    return ret;
}

void VContainer::insert(string key, Element* pElement) throw ()
{
    if(pElement->get_key() == key)
    {
        if(!this->find(key))
        {
            elements.push_back(pElement);
        }
        else
        {
            throw Exception ("Map::", __PRETTY_FUNCTION__, "Element Already exists", key);
        }
    }
    return;
}

bool VContainer::empty() const
{
    return this->elements.empty();
}

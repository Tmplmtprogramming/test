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
 * @file  MContainer.cpp
 * @brief This file will provide the template for C program coding
 *        standard.
 * @author : taeho07.kim
 * @date : 2017. 4. 12.
 * @version : 
 */

#include "MContainer.h"

MContainer::MContainer()
{
    // TODO Auto-generated constructor stub

}

MContainer::~MContainer()
{
    // TODO Auto-generated destructor stub
    clear();
}

size_t MContainer::size() const
{
    return elements.size();
}

void MContainer::clear()
{
    std::map<string, Element*>::iterator itr;
    for(itr = elements.begin(); itr != elements.end(); ++itr)
    {
        delete itr->second;
    }
    elements.clear();
}

Element* MContainer::get_element(const int index) throw ()
{
    Element* result = 0;
    if(index >=0 && (size_t)index < elements.size())
    {
        std::map<string, Element*>::iterator itr;
        std::map<string, Element*>::iterator insert_itr;
        itr = elements.begin();
        for(size_t i = 0; i < (size_t)index; ++i)
        {
            itr++;
        }
        result = itr->second;
    }
    else
    {
       throw Exception("MContainer::", "get", "indexOutOfBounds", index);
    }
    return result;
}

Element* MContainer::find(const string& key) throw ()
{
    Element* result = 0;
    std::map<string, Element*>::iterator itr;
    itr = this->elements.find(key);
    if(itr != elements.end())
    {
        result = itr->second;
    }
    return result;
}

void MContainer::insert(string key, Element* pElement) throw ()
{
    elements.insert(std::make_pair(key, pElement));
}

bool MContainer::empty() const
{
    return this->elements.empty();
}

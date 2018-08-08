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
 * @file  VContainer.h
 * @brief This file will provide the template for C program coding
 *        standard.
 * @author : taeho07.kim
 * @date : 2017. 4. 12.
 * @version : 
 */

#ifndef SRC_VCONTAINER_H_
#define SRC_VCONTAINER_H_

#include <vector>
#include "Container.h"


/**
 *
 * @class 
 * @brief 
 */
class VContainer: public Container
{
private:
    std::vector<Element*> elements;

public:
    VContainer();
    virtual ~VContainer();

    virtual size_t size() const;
    virtual void clear();
    virtual Element* get_element(const int index) throw ();
    virtual Element* find(const string& key) throw ();
    virtual void insert(string key, Element* pElement) throw ();
    virtual bool empty() const;
};

#endif /* SRC_VCONTAINER_H_ */

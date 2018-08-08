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
 * @file  MContainer.h
 * @brief This file will provide the template for C program coding
 *        standard.
 * @author : taeho07.kim
 * @date : 2017. 4. 12.
 * @version : 
 */

#ifndef SRC_MCONTAINER_H_
#define SRC_MCONTAINER_H_

#include <map>
#include "Container.h"


/**
 *
 * @class 
 * @brief 
 */
class MContainer: public Container
{
private:
    std::map<string, Element*> elements;
public:
    MContainer();
    virtual ~MContainer();

    virtual size_t size() const;
    virtual void clear();
    virtual Element* get_element(const int index) throw ();
    virtual Element* find(const string& key) throw ();
    virtual void insert(string key, Element* pElement) throw ();
    virtual bool empty() const;
};

#endif /* SRC_MCONTAINER_H_ */

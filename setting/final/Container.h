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
 * @file Container.h
 * @brief 
 * @author: 
 * @date: 
 * @version: 
 */

/*
 * Map.h
 *
 *  Created on: 2017. 4. 9.
 *      Author: choi.sungwoon
 */

#ifndef MAP_H_
#define MAP_H_

#include "Exception.h"
#include "Element.h"

#include <string>
using namespace std;


/**
 *
 * @class 
 * @brief 
 */
class Container
{
private:
public:

/**
 *
 */
    Container() {}

/**
 *
 * @return 
 */
    virtual ~Container() {}

    virtual size_t size() const = 0;
    virtual void clear() = 0;

    virtual Element* get_element(const int index) throw () = 0;
    virtual Element* find(const string& key) throw () = 0;
    virtual void insert(string key, Element* pElement) throw () = 0;
    virtual bool empty() const = 0;
};

#endif /* MAP_H_ */

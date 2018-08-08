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

class Container
{
private:
public:
    Container() {}
    virtual ~Container() {}

    virtual size_t size() const = 0;
    virtual void clear() = 0;

    virtual Element* get_element(const int index) throw () = 0;
    virtual Element* find(const string& key) throw () = 0;
    virtual void insert(string key, Element* pElement) throw () = 0;
    virtual bool empty() const = 0;
};

#endif /* MAP_H_ */

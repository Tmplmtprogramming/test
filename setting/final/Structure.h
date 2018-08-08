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
 * @file Structure.h
 * @brief 
 * @author: 
 * @date: 
 * @version: 
 */

/*
 * Structure.h
 *
 *  Created on: 2017. 4. 1.
 *      Author: choi.sungwoon
 */

#ifndef STRUCTURE_H_
#define STRUCTURE_H_ "STRUCTURE_H_"

#include "Element.h"
#include <string>

#include "Container.h"
//#include <map>

using namespace std;


/**
 *
 * @class 
 * @brief 
 */
class Structure: public Element
{
protected:
    Container* elements;
public:
    Structure();
    virtual ~Structure();

    virtual ENodeType get_type();

    void clear_elements();
    Element *get_element(const string& key) throw ();
    void add_element(Element *pElement) throw ();

    virtual void read(Lex& lex, const string& key) throw ();
    virtual void write(Lex& lex) throw ();
private:
    virtual bool prepare_elementStorage();
};

#endif /* STRUCTURE_H_ */

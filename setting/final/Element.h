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
 * @file Element.h
 * @brief 
 * @author: 
 * @date: 
 * @version: 
 */

/*
 * Element.h
 *
 *  Created on: 2017. 4. 2.
 *      Author: choi.sungwoon
 */

#ifndef ELEMENT_H_
#define ELEMENT_H_ "ELEMENT_H_"

#include "Exception.h"
#include "Lex.h"
#include <string>
using namespace std;


/**
 *
 * @enum 
 * @brief 
 */
enum ENodeType {eElement, eStructure, eArray, eNone};


/**
 *
 * @class 
 * @brief 
 */
class Element {
private:
	string key;
	string value;
public:
	Element();
	virtual ~Element();

	virtual ENodeType get_type();

	const string& get_key() const;
	void set_key(const string& key);

	const string& get_value() const;
	void set_value(const string& value);

	virtual void read(Lex& lex, const string& key) throw();
	virtual void write(Lex& lex) throw();


/**
 *
 * @param rhs 
 * @return 
 */
	const bool operator==(const Element* rhs) const
    {
	    return ((this->get_key() == rhs->get_key()));
    }
};

#endif /* ELEMENT_H_ */

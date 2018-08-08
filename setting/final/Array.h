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
 * @file Array.h
 * @brief 
 * @author: 
 * @date: 
 * @version: 
 */

/*
 * Array.h
 *
 *  Created on: 2017. 4. 6.
 *      Author: choi.sungwoon
 */

#ifndef ARRAY_H_
#define ARRAY_H_

#include "Structure.h"


/**
 *
 * @class 
 * @brief 
 */
class Array: public Structure
{
public:
    Array();
    virtual ~Array();

    virtual ENodeType get_type();

    void read(Lex& lex, const string& key) throw ();
    void write(Lex& lex) throw ();
private:
    virtual bool prepare_elementStorage();
};

#endif /* ARRAY_H_ */

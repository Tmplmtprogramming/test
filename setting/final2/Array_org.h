/*
 * Array.h
 *
 *  Created on: 2017. 4. 6.
 *      Author: choi.sungwoon
 */

#ifndef ARRAY_H_
#define ARRAY_H_

#include "Structure.h"

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

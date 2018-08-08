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

enum ENodeType {eElement, eStructure, eArray, eNone};

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

	const bool operator==(const Element* rhs) const
    {
	    return ((this->get_key() == rhs->get_key()));
    }
};

#endif /* ELEMENT_H_ */

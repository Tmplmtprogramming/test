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

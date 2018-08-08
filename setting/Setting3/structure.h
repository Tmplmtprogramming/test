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
 * @file  structure.h
 * @brief This file will provide the template for C program coding
 *        standard.
 * @author : taeho07.kim
 * @date : 2017. 4. 3.
 * @version : 
 */

#ifndef SRC_STRUCTURE_H_
#define SRC_STRUCTURE_H_

#include <vector>

#include "element.h"

class Structure: public Element
{
public:
    Structure();
    virtual ~Structure();

    virtual bool is_structure()
    {
        return true;
    }

    size_t push_element(Element* element)
    {
        this->elements.push_back(element);
        return elements.size();
    }

    unsigned int get_myDepth() const
    {
        return my_depth;
    }

    void set_myDepth(unsigned int myDepth)
    {
        my_depth = myDepth;
    }

    virtual void print()
    {
#if 0
        string spaces;
        spaces.append(get_myDepth(), ' ');
        string inside_spaces;
        inside_spaces.append(get_myDepth()+1, ' ');

        std::cout << spaces << this->name << std::endl;
        std::cout << spaces << "{" << std::endl;
        for(size_t i = 0; i < elements.size(); ++i)
        {
            cout << inside_spaces;
            elements[i]->print();
        }
        std::cout << spaces << "}" << std::endl;
#else
        for (size_t j = 0; j < this->get_myDepth(); ++j) { cout << " "; }
        cout << this->name << endl;
        for (size_t j = 0; j < this->get_myDepth(); ++j) { cout << " "; }
        cout << "{" << std::endl;
        for (size_t i = 0; i < elements.size(); ++i)
        {
            for (size_t j = 0; j < this->get_myDepth(); ++j) { cout << " "; }
            if(elements[i]->is_structure())
            {
               elements[i]->print();
            }
            else
            {
                std::cout << " ";
               elements[i]->print();
            }
        }
        for (size_t j = 0; j < this->get_myDepth(); ++j) { cout << " "; }
        std::cout << "}" << std::endl;
#endif
    }

private:
    std::vector<Element*> elements;
    unsigned int my_depth;
};

#endif /* SRC_STRUCTURE_H_ */

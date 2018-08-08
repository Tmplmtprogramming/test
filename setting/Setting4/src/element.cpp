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
 * @file  element.cpp
 * @brief This file will provide the template for C program coding
 *        standard.
 * @author : taeho07.kim
 * @date : 2017. 4. 3.
 * @version : 
 */

#include "element.h"

Element::Element(string name, string value) :
        name(name), my_depth(0), value(value)
{
    // TODO Auto-generated constructor stub

}

Element::~Element()
{
    // TODO Auto-generated destructor stub
}

Element::Element() :
        name(), my_depth(0), value()
{
}

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
 * @file  structure.cpp
 * @brief This file will provide the template for C program coding
 *        standard.
 * @author : taeho07.kim
 * @date : 2017. 4. 3.
 * @version : 
 */

#include "structure.h"

Structure::Structure() :
        Element(), elements()
{
    // TODO Auto-generated constructor stub

}

Structure::~Structure()
{
    // TODO Auto-generated destructor stub
#if USE_VECTOR
    for(std::vector<Element*>::iterator itr = elements.begin(); itr != elements.end(); ++itr)
    {
        delete (*itr);
    }
#else
    for (std::map<string, Element*>::iterator itr = elements.begin(); itr != elements.end(); ++itr)
    {
        delete itr->second;
    }
#endif
    elements.clear();
}


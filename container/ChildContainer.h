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
 * @file  ChildContainer.h
 * @brief This file will provide the template for C program coding
 *        standard.
 * @author : taeho07.kim
 * @date : 2017. 4. 11.
 * @version : 
 */

#ifndef CHILDCONTAINER_H_
#define CHILDCONTAINER_H_

#include "Container.h"

template<typename T>
class ChildContainer: public Container
{
public:
    ChildContainer();
    virtual ~ChildContainer();
};

#endif /* CHILDCONTAINER_H_ */

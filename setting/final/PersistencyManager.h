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
 * @file PersistencyManager.h
 * @brief 
 * @author: 
 * @date: 
 * @version: 
 */

/*
 * Main.h
 *
 *  Created on: 2017. 4. 2.
 *      Author: choi.sungwoon
 */

#ifndef PERSISTENCYMANAGER_H_
#define PERSISTENCYMANAGER_H_

#include "Parser.h"
#include "VideoManager.h"

/**
 *
 * @class 
 * @brief 
 */
class PersistencyManager
{
private:
    Parser parser;

    VideoManager videoManager1;
    VideoManager videoManager2;
    VideoManager videoManager3;
    A a;

public:
    PersistencyManager();
    ~PersistencyManager();
    void read();
    void write();
};

#endif /* PERSISTENCYMANAGER_H_ */

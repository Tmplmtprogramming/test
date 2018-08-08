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
 * @file Main.cpp
 * @brief 
 * @author: 
 * @date: 
 * @version: 
 */

/*
 * main.cpp
 *
 *  Created on: 2017. 3. 30.
 *      Author: choi.techwin
 */

#include "Exception.h"
#include "PersistencyManager.h"

#include <vector>
using namespace std;
#include "VContainer.h"


/**
 *
 * @return 
 */
int main()
{
    PersistencyManager persistencyManager;
    persistencyManager.read();
    persistencyManager.write();
}


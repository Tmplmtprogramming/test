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
 * @file  PersistencyManager.cpp
 * @brief This file will provide the template for C program coding
 *        standard.
 * @author : taeho07.kim
 * @date : 2017. 4. 11.
 * @version : 
 */

#include "PersistencyManager.h"

PersistencyManager::PersistencyManager() :
        parser(), videoManager1(), videoManager2(), videoManager3(), a()
{
}

PersistencyManager::~PersistencyManager()
{
}

void PersistencyManager::read()
{
    try
    {
//        parser.openIn("rsc", "input");
        parser.openIn("rsc", "test");
        // read file and build parse tree
        parser.read();
        // load data to a class
//        parser.load(videoManager1, "videoManager1");
//			parser.load(videoManager3, "videoManager3");
//			parser.load(videoManager2, "videoManager2");
        parser.load(a, "a");
        parser.closeIn();
    } catch (Exception& e)
    {
        e.what();
    }
}

void PersistencyManager::write()
{
    try
    {
        parser.openOut("rsc", "output");
        // load data to the parser
//			parser.store(videoManager3, "videoManager3");
//			parser.store(videoManager2, "videoManager2");
//        parser.store(videoManager1, "videoManager1");
        parser.store(a, "a");
        // write to the file
        parser.write();
        parser.closeOut();
    } catch (Exception& e)
    {

        e.what();
    }
}

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

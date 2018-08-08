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

int main()
{
    PersistencyManager persistencyManager;
    persistencyManager.read();
    persistencyManager.write();
}


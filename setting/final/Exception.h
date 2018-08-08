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
 * @file Exception.h
 * @brief 
 * @author: 
 * @date: 
 * @version: 
 */

/*
 * exception.h
 *
 *  Created on: 2017. 3. 31.
 *      Author: choi.sungwoon
 */

#ifndef EXCEPTION_H_
#define EXCEPTION_H_ "EXCEPTION_H_"

#include <string>
#include <exception>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;


/**
 *
 * @class 
 * @brief 
 */
class Message
{
private:
    string message;


/**
 *
 * @param moduleName 
 * @param functionName 
 * @param cause 
 */
    void append(string moduleName, string functionName, string cause)
    {
        message.clear();
        message.append(moduleName);
        message.append("::");
        message.append(functionName);
        message.append(" ");
        message.append(cause);
    }

/**
 *
 * @param moduleName 
 * @param functionName 
 * @param cause1 
 * @param cause2 
 */
    void append(string moduleName, string functionName, string cause1, string cause2)
    {
        message.clear();
        message.append(moduleName);
        message.append("::");
        message.append(functionName);
        message.append(" ");
        message.append(cause1);
        message.append(" ");
        message.append(cause2);
    }

/**
 *
 * @param moduleName 
 * @param functionName 
 * @param cause1 
 * @param cause2 
 */
    void append(string moduleName, string functionName, string cause1, int cause2)
    {
        message.clear();
        message.append(moduleName);
        message.append("::");
        message.append(functionName);
        message.append(" ");
        message.append(cause1);
        message.append(" ");
        stringstream ss;
        ss << cause2;
        string s;
        ss >> s;
        message.append(s);
    }

public:

/**
 *
 */
    Message()
    {
    }

/**
 *
 * @param moduleName 
 * @param functionName 
 * @param cause 
 */
    Message(string moduleName, string functionName, string cause)
    {
        append(moduleName, functionName, cause);
    }


/**
 *
 * @return 
 */
    const string& get()
    {
        return this->message;
    }

/**
 *
 * @param moduleName 
 * @param functionName 
 * @param cause 
 */
    void set(string moduleName, string functionName, string cause)
    {
        append(moduleName, functionName, cause);
    }

/**
 *
 * @param moduleName 
 * @param functionName 
 * @param cause1 
 * @param cause2 
 */
    void set(string moduleName, string functionName, string cause1, string cause2)
    {
        append(moduleName, functionName, cause1, cause2);
    }

/**
 *
 * @param moduleName 
 * @param functionName 
 * @param cause1 
 * @param cause2 
 */
    void set(string moduleName, string functionName, string cause1, int cause2)
    {
        append(moduleName, functionName, cause1, cause2);
    }


/**
 *
 * @param cause 
 */
    void setCause(string& cause)
    {
        this->message.append(cause);
    }


/**
 *
 */
    void show()
    {
        cout << message << endl;
    }

/**
 *
 * @param message 
 */
    void show(string message)
    {
        this->message = message;
        cout << message << endl;
    }

/**
 *
 * @param moduleName 
 * @param functionName 
 * @param cause 
 */
    void show(string moduleName, string functionName, string cause)
    {
        append(moduleName, functionName, cause);
        cout << message << endl;
    }

/**
 *
 * @param moduleName 
 * @param functionName 
 * @param cause1 
 * @param cause2 
 */
    void show(string moduleName, string functionName, string cause1, string cause2)
    {
        append(moduleName, functionName, cause1, cause2);
        cout << message << endl;
    }

};

extern Message gMessage;


/**
 *
 * @class 
 * @brief 
 */
class Exception: public exception
{
public:

/**
 *
 * @param cause 
 */
    Exception(string cause)
    {
        gMessage.setCause(cause);
    }

/**
 *
 * @param moduleName 
 * @param functionName 
 * @param cause 
 */
    Exception(string moduleName, string functionName, string cause)
    {
        gMessage.set(moduleName, functionName, cause);
    }

/**
 *
 * @param moduleName 
 * @param functionName 
 * @param cause1 
 * @param cause2 
 */
    Exception(string moduleName, string functionName, string cause1, string cause2)
    {
        gMessage.set(moduleName, functionName, cause1, cause2);
    }

/**
 *
 * @param moduleName 
 * @param functionName 
 * @param cause1 
 * @param cause2 
 */
    Exception(string moduleName, string functionName, string cause1, int cause2)
    {
        gMessage.set(moduleName, functionName, cause1, cause2);
    }

/**
 *
 * @return 
 */
    virtual ~Exception() throw ()
    {
    }


/**
 *
 * @param cause 
 */
    void setCause(string& cause)
    {
        gMessage.setCause(cause);
    }

/**
 *
 * @return 
 */
    const char* what() const throw ()
    {
        return gMessage.get().c_str();
    }
};

#endif /* EXCEPTION_H_ */

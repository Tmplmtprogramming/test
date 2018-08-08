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

class Message
{
private:
    string message;

    void append(string moduleName, string functionName, string cause)
    {
        message.clear();
        message.append(moduleName);
        message.append("::");
        message.append(functionName);
        message.append(" ");
        message.append(cause);
    }
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
    Message()
    {
    }
    Message(string moduleName, string functionName, string cause)
    {
        append(moduleName, functionName, cause);
    }

    const string& get()
    {
        return this->message;
    }
    void set(string moduleName, string functionName, string cause)
    {
        append(moduleName, functionName, cause);
    }
    void set(string moduleName, string functionName, string cause1, string cause2)
    {
        append(moduleName, functionName, cause1, cause2);
    }
    void set(string moduleName, string functionName, string cause1, int cause2)
    {
        append(moduleName, functionName, cause1, cause2);
    }

    void setCause(string& cause)
    {
        this->message.append(cause);
    }

    void show()
    {
        cout << message << endl;
    }
    void show(string message)
    {
        this->message = message;
        cout << message << endl;
    }
    void show(string moduleName, string functionName, string cause)
    {
        append(moduleName, functionName, cause);
        cout << message << endl;
    }
    void show(string moduleName, string functionName, string cause1, string cause2)
    {
        append(moduleName, functionName, cause1, cause2);
        cout << message << endl;
    }

};

extern Message gMessage;

class Exception: public exception
{
public:
    Exception(string cause)
    {
        gMessage.setCause(cause);
    }
    Exception(string moduleName, string functionName, string cause)
    {
        gMessage.set(moduleName, functionName, cause);
    }
    Exception(string moduleName, string functionName, string cause1, string cause2)
    {
        gMessage.set(moduleName, functionName, cause1, cause2);
    }
    Exception(string moduleName, string functionName, string cause1, int cause2)
    {
        gMessage.set(moduleName, functionName, cause1, cause2);
    }
    virtual ~Exception() throw ()
    {
    }

    void setCause(string& cause)
    {
        gMessage.setCause(cause);
    }
    const char* what() const throw ()
    {
        return gMessage.get().c_str();
    }
};

#endif /* EXCEPTION_H_ */

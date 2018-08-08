/*
 * Lex.h
 *
 *  Created on: 2017. 3. 30.
 *      Author: choi.techwin
 */

#ifndef LEX_H_
#define LEX_H_

#include <string>
#include <sstream>
#include <fstream>

#include <stack>
#include <exception>

using namespace std;

#define BLANK " \t\n\r"
#define BEGIN '{'
#define END '}'
#define PERIOD '.'

class Lex
{
private:
    char lookahead;

    bool isBlank(char c)
    {
        if (string(BLANK).find(c) != string::npos)
            return true;
        return false;
    }
    bool isDelimeter(char c)
    {
        if (isBlank(c))
            return true;
        if (c == BEGIN || c == END)
            return true;
        return false;
    }
    bool isDigit(char c)
    {
        if (c >= '0' && c <= '9')
            return true;
        return false;
    }
    bool isPeriod(char c)
    {
        if (c == PERIOD)
            return true;
        return false;
    }

    void readBlank(ifstream &fin)
    {
        fin.get(this->lookahead);
        while (isBlank(this->lookahead) && !fin.eof())
        {
            if (fin.eof())
                return;
            fin.get(this->lookahead);
        }
    }
    void readDelimeter(ifstream &fin)
    {
        fin.get(this->lookahead);
        while (isDelimeter(this->lookahead) && !fin.eof())
        {
            if (fin.eof())
                return;
            fin.get(this->lookahead);
        }
    }
    string readDigit(ifstream &fin)
    {
        string digits;
        fin.get(this->lookahead);
        while (isDigit(this->lookahead))
        {
            if (fin.eof())
                return digits;
            digits.append(1, lookahead);
            fin.get(this->lookahead);
        }
        return digits;
    }

public:
    Lex() :
            lookahead(0)
    {
    }
    virtual ~Lex()
    {
    }

    int readInt(ifstream &fin)
    {
        string token;
        this->readBlank(fin);
        while (isDigit(this->lookahead) && !fin.eof())
        {
            token.append(1, this->lookahead);
            fin.get(this->lookahead);
        }
        if (token.empty())
            throw new exception();

        stringstream ss;
        ss << token;
        int result;
        ss >> result;
        return result;
    }

    float readFloat(ifstream &fin)
    {
        string token;
        this->readBlank(fin);
        while (isDigit(this->lookahead) && !fin.eof())
        {
            token.append(1, this->lookahead);
            fin.get(this->lookahead);
        }
        if (token.empty())
            throw "Token is Empty!!";

        stringstream ss;
        ss << token;
        float result;
        ss >> result;
        return result;
    }

    string readString(ifstream& fin)
    {
        string token;
        this->readDelimeter(fin);
        while (!this->isDelimeter(lookahead) && !fin.eof())
        {
            token.append(1, this->lookahead);
            fin.get(this->lookahead);
        }
        if (token.empty())
        {
//            throw exception;
            throw "Token is Empty!!";
        }

        return token;
    }

    string readBegin(ifstream& fin)
    {
        string token;
        this->readBlank(fin);
        while (!fin.eof())
        {
            if (this->lookahead != BEGIN)
            {
                fin.get(this->lookahead);
            }
            else
            {
                token.append(1, this->lookahead);
                fin.get(this->lookahead);
                break;
            }
        }
        if (token.empty())
        {
//            throw exception;
            throw "Token is Empty.";
        }

        return token;
    }

    size_t readStructure(ifstream& fin, std::stack<string>& stk)
    {
        string token;
        while (!fin.eof())
        {
            if (!(this->isBlank(this->lookahead)))
            {
                if (this->lookahead == BEGIN || this->lookahead == END)
                {
                    if (!token.empty())
                    {
                        stk.push(token);
                    }
                    stk.push(std::string(1, lookahead));
                    token.clear();
                }
                else
                {
                    token.append(1, this->lookahead);
                }
            }
            else
            {
                if (!token.empty())
                {
                    stk.push(token);
                    token.clear();
                }
            }
            fin.get(this->lookahead);
        }
        return stk.size();
    }
};

#endif /* LEX_H_ */

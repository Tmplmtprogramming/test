/*
 * Lex.h
 *
 *  Created on: 2017. 3. 30.
 *      Author: choi.techwin
 */

#ifndef LEX_H_
#define LEX_H_ "LEX_H_"

#include "Exception.h"

#include <string>
#include <fstream>
using namespace std;

#define BLANKS " \t\n\r"
#define SPACE ' '
#define TAB '\t'
#define NEWLINE '\n'
#define BEGIN '{'
#define END '}'
#define INDEXBEGIN '['
#define INDEXEND ']'
#define PERIOD '.'
#define ZERO '0'
#define NINE '9'

#define PATHSEPARATOR "/"
#define EXTENSION ".txt"

class Lex
{
private:
    ifstream fin;
    ofstream fout;

    char lookahead;
    string tabs;

    bool is_blank(const char c)
    {
        string token;
        token.append(BLANKS);
        if (token.find(c) != string::npos)
            return true;
        return false;
    }
    bool is_begin(const char c)
    {
        if (c == BEGIN)
            return true;
        return false;
    }
    bool is_end(const char c)
    {
        if (c == END)
            return true;
        return false;
    }
    bool is_indexBegin(const char c)
    {
        if (c == INDEXBEGIN)
            return true;
        return false;
    }
    bool is_indexEnd(const char c)
    {
        if (c == INDEXEND)
            return true;
        return false;
    }

    bool is_delimeter(const char c)
    {
        if (this->is_blank(c))
            return true;
        if (this->is_begin(c))
            return true;
        if (this->is_end(c))
            return true;
        if (this->is_indexBegin(c))
            return true;
        if (this->is_indexEnd(c))
            return true;
        return false;
    }
    bool is_digit(const char c)
    {
        if (c >= ZERO && c <= NINE)
            return true;
        return false;
    }
    bool is_period(const char c)
    {
        if (c == PERIOD)
            return true;
        return false;
    }

    void read_blanks()
    {
        while (this->is_blank(this->lookahead) && !fin.eof())
        {
            fin.get(this->lookahead);
        }
    }
    inline string read_digits()
    {
        string token;
        while (this->is_digit(this->lookahead) && !fin.eof())
        {
            token.append(1, this->lookahead);
            fin.get(this->lookahead);
        }
        return token;
    }
    inline string read_period()
    {
        string token;
        if (this->is_period(this->lookahead) && !fin.eof())
        {
            token.append(1, this->lookahead);
            fin.get(this->lookahead);
        }
        return token;
    }
    inline string read_chars()
    {
        string token;
        while (!this->is_delimeter(this->lookahead) && !fin.eof())
        {
            token.append(1, this->lookahead);
            fin.get(this->lookahead);
        }
        return token;
    }
    inline string read_beginToken()
    {
        string token;
        if (this->is_begin(this->lookahead) && !fin.eof())
        {
            token.append(1, this->lookahead);
            fin.get(this->lookahead);
        }
        return token;
    }
    inline string read_endToken()
    {
        string token;
        if (this->is_end(this->lookahead) && !fin.eof())
        {
            token.append(1, this->lookahead);
            fin.get(this->lookahead);
        }
        return token;
    }
    inline string read_indexBeginToken()
    {
        string token;
        if (this->is_indexBegin(this->lookahead) && !fin.eof())
        {
            token.append(1, this->lookahead);
            fin.get(this->lookahead);
        }
        return token;
    }
    inline string read_indexEndToken()
    {
        string token;
        if (this->is_indexEnd(this->lookahead) && !fin.eof())
        {
            token.append(1, this->lookahead);
            fin.get(this->lookahead);
        }
        return token;
    }

    void insert_tabIndentation()
    {
        this->tabs.push_back(TAB);
    }
    void remove_tabIndentation()
    {
        this->tabs.erase(0, 1);
    }

    inline string get_fullName(const string& path, const string& objectName)
    {
        string fullName;
        fullName.append(path);
        fullName.append(PATHSEPARATOR);
        fullName.append(objectName);
        fullName.append(EXTENSION);
        return fullName;
    }

public:
    Lex() :
            lookahead(BLANKS[0])
    {
    }
    virtual ~Lex()
    {
    }

    void open_in(const string& path, const string& fileName) throw ()
    {
        string fullName = this->get_fullName(path, fileName);

        this->fin.open(fullName.c_str());
        if (!this->fin.is_open())
            throw Exception(LEX_H_, "openIn", fullName);
    }
    bool eof()
    {
        return fin.eof();
    }
    void close_in()
    {
        this->fin.close();
    }

    inline string read_begin()
    {
        string token;
        this->read_blanks();
        token = this->read_beginToken();
        return token;
    }
    inline string read_end()
    {
        string token;
        this->read_blanks();
        token = this->read_endToken();
        return token;
    }
    inline string read_indexBegin()
    {
        string token;
        this->read_blanks();
        token = this->read_indexBeginToken();
        return token;
    }
    inline string read_indexEnd()
    {
        string token;
        this->read_blanks();
        token = this->read_indexEndToken();
        return token;
    }

    inline string read_int()
    {
        string token;
        this->read_blanks();
        token = this->read_digits();
        return token;
    }
    inline string read_float()
    {
        string token;
        this->read_blanks();
        token.append(this->read_digits());
        token.append(this->read_period());
        token.append(this->read_digits());
        return token;
    }
    inline string read_string()
    {
        string token;
        this->read_blanks();
        token = this->read_chars();
        return token;
    }

    void open_out(const string& path, const string& fileName) throw ()
    {
        string fullName = this->get_fullName(path, fileName);

        this->fout.open(fullName.c_str());
        if (!this->fout.is_open())
            throw Exception(LEX_H_, "openOut", fullName);
        this->tabs.clear();
    }
    void close_out()
    {
        this->fout.close();
    }

    void write_begin()
    {
        fout << BEGIN << NEWLINE;
        insert_tabIndentation();
    }
    void write_end()
    {
        remove_tabIndentation();
        fout << this->tabs << END << NEWLINE;
    }
    void write_indexBegin()
    {
        fout << INDEXBEGIN;
    }
    void write_indexEnd()
    {
        fout << INDEXEND;
    }

    void write_key(const string& token)
    {
        fout << this->tabs << token;
    }
    void write_value(const string& token)
    {
        fout << SPACE << token << NEWLINE;
    }
    void write_space()
    {
        fout << SPACE;
    }
    void write_tab()
    {
        fout << this->tabs;
    }
    void write_endl()
    {
        fout << NEWLINE;
    }
};

#endif /* LEX_H_ */

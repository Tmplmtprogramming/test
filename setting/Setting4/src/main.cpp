/*
 * main.cpp
 *
 *  Created on: 2017. 3. 30.
 *      Author: choi.techwin
 */

#include "Parser.h"

#include <iostream>
#include <exception>
using namespace std;

int main(int argc, char** argv)
{
    try
    {
        Parser parser;
#if 1
        string filename;
        if (argc == 2)
        {
            filename = argv[1];
        }
        else
        {
            filename = "rsc/auth_setting.txt";
        }
        parser.open(filename);
        parser.readBody();
        parser.printBody();

        string i("keyPw");
        Element* ret = parser.find(i);
        if(ret)
        {
            cout << "ret->value = " << ret->get_value() << std::endl;
        }
        else
        {
            cout << "Element is not found with key(" << i << ")" << std::endl;
        }

        Element* new_element = new Element("whatisit", "TaehoKim");
        parser.insert_element("TlsSetitng", new_element);

        Structure* new_structure = new Structure();
        new_structure->set_name("SYLee");
        new_structure->push_element(new Element("Jinhong", "Jinseo"));

        parser.insert_element("IEEE8021xSetting", new_structure);

        Structure* new_2nd_structure = new Structure();
        new_2nd_structure->set_name("Hyundai");
        new_2nd_structure->push_element(new Element("Avante", "HD"));

        Structure* new_3rd_structure = new Structure();
        new_3rd_structure->set_name("Benz");
        new_3rd_structure->push_element(new Element("S-Class", "330"));
        new_2nd_structure->push_element(new_3rd_structure);

        parser.insert_element("TlsSetitng", new_2nd_structure);

        ofstream fos;
        string write_file(filename);
        write_file.append("_saved");
        fos.open(write_file.c_str(), std::ofstream::out | std::ofstream::app);
        parser.writeBody(fos);
        fos.close();

#else
        parser.open("rsc/data.txt");
        int result;
        result = parser.readInt();
        cout << result << endl;
        result = parser.readInt();
        cout << result << endl;
        result = parser.readInt();
        cout << result << endl;

#endif
        parser.close();
    } catch (exception& e)
    {
        cout << e.what();
    }
    catch (const char* e)
    {
	    cout << e;
    }
    return 0;
}


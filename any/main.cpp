#include "any.h"

#include <iostream>
#include <vector>

int main()
{
	std::vector<any> some_values;

	some_values.push_back(10);
	const char* c_str = "Hello world!";
	some_values.push_back(c_str);
	some_values.push_back(std::string("Wow!"));

	//std::string& s = any_cast<std::string&>(some_values.back());
	std::string s = any_cast<std::string>(some_values.back());

	s += " That is greate!\n";

	std::cout << s;

	any variable(std::string("Hello there!"));
	std::string s1 = any_cast<std::string>(variable);
	std::string* s2 = any_cast<std::string>(&variable);

	std::cout << s1 << std::endl;
	if(s2)
	{
	std::cout << s2->c_str() << std::endl;
	}
	else
	{
		std::cout << "s2 is NULL" << std::endl;
	}
	some_values.push_back(3.14);
        float pi = any_cast<const double>(some_values.back());
        //float pi = unsafe_any_cast<float>(some_values.back());
	std::cout << "Pi : " << pi << std::endl;
	return 0;
}

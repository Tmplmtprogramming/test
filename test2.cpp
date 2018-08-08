#include <sstream>
#include <iostream>
using namespace std;
int main()
{
	string string_test = "ffff";
	stringstream s2(string_test);
	unsigned int x;
	unsigned int val;

	//s2 <<  string_test;
	s2 >> std::hex >>  x;

//	cout << static_cast<int>(x) << endl;;
	cout << x << endl;;

	return 0;
}

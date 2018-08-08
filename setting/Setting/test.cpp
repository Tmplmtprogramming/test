#include <fstream>
#include <vector>
#include <iostream>

using namespace std;

int main()
{
	fstream fs;
	fs.open("test.txt", std::fstream::in | std::fstream::out | std::fstream::app);

	string text;

	vector<string> text_vec;

	fs >> text;
	cout << text.length() << endl;
	text_vec.push_back(text);

	fs >> text;
	cout << text.length() << endl;
	text_vec.push_back(text);

	fs >> text;
	cout << text.length() << endl;
	text_vec.push_back(text);

	fs >> text;
	cout << text.length() << endl;
	text_vec.push_back(text);

	cout << text_vec[0] << endl;
	cout << text_vec[1] << endl;
	cout << text_vec[2] << endl;
	cout << text_vec[3] << endl;
	return 0;
}

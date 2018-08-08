#include <iostream>

using namespace std;

class A
{
	int a;
	int b;
	public:
	virtual size_t get_size()
	{
		cout << "sizeof(A)" << sizeof(*this) << endl;
		return sizeof(*this);
	}
};

class B : public A
{
	int c;
	int d;
	public:
	size_t get_size()
	{
		A::get_size();
		cout << "sizeof(B)" << sizeof(*this) << endl;
		return sizeof(*this);
	}
};


int main()
{
	A* a = 0;
	B b;
	a = &b;
	//b.get_size();
	a->get_size();
	return 0;
}


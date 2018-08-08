#include <memory>
#include <iostream>

using namespace std;

class Base
{
	public:
	Base()
	{
		cout << __PRETTY_FUNCTION__ << std::endl;
	}
	virtual ~Base()
	{
		cout << __PRETTY_FUNCTION__ << std::endl;
	}
};

class Derived : public Base
{
	public:
	Derived() : Base()
	{
		cout << __PRETTY_FUNCTION__ << std::endl;
	}

	virtual ~Derived()
	{
		cout << __PRETTY_FUNCTION__ << std::endl;
	}
};


int main()
{
	shared_ptr<Base> a_ptr((Base*)new Derived);
	//shared_ptr<Base> a_ptr(new Derived);
	return 0;
}

#include <map>
#include <iostream>
#include <functional>

using namespace std;

int a ()
{
	cout << "a()" << std::endl;
	    return 0;
}

int b()
{
	cout << "b()" << std::endl;
	    return 0;
}

int c()
{
	cout << "c()" << std::endl;
	    return 0;
}

enum Item
{
	A, B, C
};

#if 0
typedef int (*function_ptr)();
#endif

class FunctionClass
{
	public:
	FunctionClass() : function_map()
	{
		this->function_map.insert(std::make_pair<char, std::mem_fun_t<int, FunctionClass> >('a', std::mem_fun(&FunctionClass::a)));
		this->function_map.insert(std::make_pair<char, std::mem_fun_t<int, FunctionClass> >('b', std::mem_fun(&FunctionClass::b)));
		this->function_map.insert(std::make_pair<char, std::mem_fun_t<int, FunctionClass> >('c', std::mem_fun(&FunctionClass::c)));
		this->function_map.insert(std::make_pair<char, std::mem_fun_t<int, FunctionClass> >('d', std::mem_fun(&FunctionClass::d)));
		this->function_map.insert(std::make_pair<char, std::mem_fun_t<int, FunctionClass> >('e', std::mem_fun(&FunctionClass::e)));
		this->function_map.insert(std::make_pair<char, std::mem_fun_t<int, FunctionClass> >('f', std::mem_fun(&FunctionClass::f)));
		this->function_map.insert(std::make_pair<char, std::mem_fun_t<int, FunctionClass> >('g', std::mem_fun(&FunctionClass::g)));
		this->function_map.insert(std::make_pair<char, std::mem_fun_t<int, FunctionClass> >('h', std::mem_fun(&FunctionClass::h)));
	}
	int class_function(char command)
	{
		int result = -1;
		std::map<char, std::mem_fun_t<int, FunctionClass> >::iterator itr;
		itr = this->function_map.find(command);
		if(itr != function_map.end())
		{
			result = (itr->second)(this);
		}
		return result;
	}

	private:
	int a() { std::cout << __FUNCTION__ << std::endl; return 0; }
	int b() { std::cout << __FUNCTION__ << std::endl; return 1; }
	int c() { std::cout << __FUNCTION__ << std::endl; return 2; }
	int d() { std::cout << __FUNCTION__ << std::endl; return 3; }
	int e() { std::cout << __FUNCTION__ << std::endl; return 4; }
	int f() { std::cout << __FUNCTION__ << std::endl; return 5; }
	int g() { std::cout << __FUNCTION__ << std::endl; return 6; }
	int h() { std::cout << __FUNCTION__ << std::endl; return 7; }

	private:
	std::map<char, std::mem_fun_t<int, FunctionClass> > function_map;
};

int main()
{
#if 0
	map<char, function_ptr> function_map;
#else
	map<char, int(*)()> function_map;
#endif

	function_map.insert(std::make_pair<char, int(*)()>('a', &a));
	function_map.insert(std::make_pair<char, int(*)()>('b', &b));
	function_map.insert(std::make_pair<char, int(*)()>('c', &c));


	(function_map['a'])();
	(function_map['b'])();
	(function_map['c'])();

	FunctionClass obj;
	obj.class_function('h');
	obj.class_function('g');
	obj.class_function('f');
	obj.class_function('e');
	obj.class_function('d');
	obj.class_function('c');
	obj.class_function('b');
	obj.class_function('a');

	return 0;
}

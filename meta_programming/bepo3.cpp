#include <iostream>

struct Counter
{
	static size_t value;
};

size_t Counter::value = 1;

template<typename T>
struct TypeID : private Counter
{
	static size_t value()
	{
		static size_t value = Counter::value++;
		return value;
	}
};

class B
{
	public:
	static const int base;
	enum events{ a = B::base };
	B()
	{
	}
};

const int B::base = TypeID<B>::value();

class A
{

	public:
	static const int base;

	enum events{ a = base };
	A()
	{
	}
};

const int A::base = TypeID<A>::value();

int main()
{
	    std::cout << TypeID<int>::value() << " " << TypeID<int*>::value() << " " << TypeID<Counter>::value() << " " << TypeID<int>::value() << " " << TypeID<A>::value() << " " << TypeID<B>::value() << " " << TypeID<A::events>::value() << " " << TypeID<B::events>::value() << std::endl;;

	    return 0;
}

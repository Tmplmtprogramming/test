#include <iostream>

using namespace std;

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
		//static size_t value = Counter::value++;
		return Counter::value++;
	}
};

//static const int a = 1;

struct A
{
	//static constexpr int EventStart = a;
	//static constexpr int EventEnd = a + 1024;
	static size_t base = TypeID<A>::value();
};

int main()
{
	cout << " " << A::base<< endl;
	return 0;
}

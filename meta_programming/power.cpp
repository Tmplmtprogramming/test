#include <iostream>

template <int base, int exponent>
struct power
{
	enum
	{
		value = base * power<base, exponent -1>::value
	};
};

template <int base>
struct power <base, 0>
{
	enum
	{
		value = 1
	};
};

int main()
{
	using namespace std;
	cout << "result = " << power<2, 30>::value << endl;
	return 0;
}

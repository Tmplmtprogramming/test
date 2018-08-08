#include <iostream>

template <typename _Ty>

struct is_int
{
	enum _value_
	{
		value = false
	};
};

template <>
struct is_int<int>
{
	enum _value_
	{
		value = true
	};
};

int main(void)
{
	using std::cout;
	using std::endl;

	cout << is_int<bool>::value << endl;
	cout << is_int<float>::value << endl;
	cout << is_int<int>::value << endl;

	return 0;
}

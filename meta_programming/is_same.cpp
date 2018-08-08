#include <iostream>

template <typename T1, typename T2>
struct is_same
{
	enum _value_ { value = false };
};

template <typename T>
struct is_same<T, T>
{
	enum _value_ { value = true };
};

int main()
{
	using std::cout;
	using std::endl;

	cout << is_same<int, float>::value << endl;
	cout << is_same<int, long>::value << endl;
	cout << is_same<int, int>::value << endl;
	return 0;
}

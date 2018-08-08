template<unsigned long N>
struct binary
{
	static unsigned const value = binary<N/10>::value << 1 | N %10;
};

template <>
struct binary<0>
{
	static unsigned const value = 0;
};

#include <iostream>
using namespace std;
int main()
{
	unsigned const one = binary<1>::value;
	cout << one << endl;
	return 0;
}

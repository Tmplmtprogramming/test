#include <iostream>

using namespace std;

template <typename T>
struct type_size
{
	enum _value_
	{
		value = sizeof(T)
	};
};

template <typename Target, typename dectype s>
struct shirink_data
{
	static Target get()
	{
		Target temp;
		memset(&temp, 0xFF, sizeof(temp));
		Target ret;
		ret = reinterpret_cast<Target>(s & temp);
		return ret;
	}

};

int main()
{
	int a;
	char b;
	float c;
	long d;
	uint32_t e;
	uint8_t f;
	uint16_t g;
	uint64_t h;
	
	cout << type_size<int>::value << endl;
	cout << type_size<char>::value << endl;
	cout << type_size<float>::value << endl;
	cout << type_size<long>::value << endl;
	cout << type_size<uint32_t>::value << endl;
	cout << type_size<uint8_t>::value << endl;
	cout << type_size<uint16_t>::value << endl;
	cout << type_size<uint64_t>::value << endl;

	char aa = 0x00;

	static uint16_t bb = 0xffee;

#if 0
	aa = (char)bb;
	cout << (int)aa << endl;
#else
	aa = shirink_data<char, bb>::get();
	cout << aa << endl;
#endif

	return 0;
}

#include <iostream>

struct Counter
{
	protected:
		static size_t value;
};

#if 1
template<typename T>
struct TypeID : Counter
{
	static size_t value()
	{
		static size_t value = Counter::value++;
		return value;
	}
};
#else
class TypeID
{
	static size_t counter;

	public:
	template <typename T>
	static size_t value()
	{
		static size_t id = TypeID::counter++;
		return id;
	}
};
#endif

class A{};
class B{};

int main()
{
	std::cout << TypeID::value<A>() << std::endl;
	std::cout << TypeID::value<B>() << std::endl;
	return 0;
}

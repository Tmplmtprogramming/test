#include <iostream>
#include <thread>

void foo()
{
	std::cout << "foo()" << std::endl;
}

void bar(int x)
{
	std::cout << "bar()" << std::endl;
}

int main()
{
	std::thread first(foo);
	std::thread second(bar,0);
	std::cout << "main, boo and bar now execute cuncurrently...\n";

	// synchronize thread:
	first.join();
	second.join();

	std::cout << "foo and bar completed.\n";

	return 0;
}

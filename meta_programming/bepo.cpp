#include <iostream>

using namespace std;


template<typename T>
struct type { static void id() { } };

template<typename T>
size_t type_id() { return reinterpret_cast<size_t>(&type<T>::id); }

struct A { };

int main ()
{
	cout << type_id<int>() << " " << type_id<int*>() << " " << type_id<A>() << " " << type_id<int>() << endl;
}

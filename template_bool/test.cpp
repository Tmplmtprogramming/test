#include <iostream>

using namespace std;

#if __cplusplus > 199711L
#define CPP11
#else
#endif
#ifdef CPP11
template <bool flag = true>
#else
template <bool flag>
#endif
int print_ab()
{
	if(flag)
	{
		cout << "A" << endl;
	}
	else
	{
		cout << "B" << endl;
	}
	return 0;
}

int main()
{
	int ret = 0;
#ifdef CPP11
	ret = print_ab();
#endif
	ret = print_ab<false>();
	ret = print_ab<true>();
	return 0;
}

#include <cstdio>

template<unsigned int N>
struct factorial
{
    enum {value = N*factorial<N-1>::value};
};

template<>
struct factorial<0>
{
    enum {value = 1};
};

int main()
{
    unsigned int val = factorial<4>::value;
    printf("val  = %d", val);
    return 0;
}

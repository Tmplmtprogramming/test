#include <iostream>

using namespace std;

template<typename T, size_t N>
void print(T (&arr)[N])
{
    for(size_t i = 0; i < N; i++)
    {
        cout << arr[i] << " ";
        if(i % 4 == 3) cout << endl;
    }
    return;
}

int main()
{
#if 0
    char* ptr = NULL;

    ptr = new char[10];
#else
    char (*ptr)[10] = (char(*)[10])new char[10];
#endif
    (*ptr)[0] = 'a';
    (*ptr)[1] = 'b';
    (*ptr)[2] = 'c';
    (*ptr)[3] = 'd';
    (*ptr)[4] = 'e';
    (*ptr)[5] = 'f';
    (*ptr)[6] = 'g';
    (*ptr)[7] = 'h';
    (*ptr)[8] = 'i';
    (*ptr)[9] = 'j';

    print(*ptr);
    return 0;
}

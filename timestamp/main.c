#include <stdio.h>

int main(int argc, char** argv)
{
    unsigned int old = 0xffffffff;
    unsigned int new = 0x1;

    printf("result = %d\n", (int)(old - new));

    unsigned int old1 = 0xffffffff;
    unsigned int new2 = 0x1;

    printf("result = %d\n", (int)(old1 - new2));

    return 0;
}

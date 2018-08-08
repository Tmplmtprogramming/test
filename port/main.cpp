#include <stdio.h>
#include <stdlib.h>

#define MAX_LOCAL_UDP_PORT 60000
#define MIN_LOCAL_UDP_PORT 40000

int main(int argc, char** argv)
{
    int nextLocalPort = 40000;
    if(argc == 2)
    {
        nextLocalPort = atoi(argv[1]);
    }
    int ret = (nextLocalPort + 1) & (~1);    // make even
    int result = 0;
    int count = 8;

//    nextLocalPort = ret + count;
    if (nextLocalPort >= MAX_LOCAL_UDP_PORT)
    {
        nextLocalPort = MIN_LOCAL_UDP_PORT;
    }

    char  buffer[256];
    for(int i = 0 ; i < 8; i++)
    {
        sprintf(buffer, "netstat -aun | grep %d", ret + i);
        result = system(buffer);
        if(result == 1)
        {
            printf("using Next port = %d\n", ret + i);
            nextLocalPort += count;
            ret += count;
            break;
        }
    }
    printf("ret = %d\n", ret);
    return 0;
}

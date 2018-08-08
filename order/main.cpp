

#include <stdint.h>
#include <iostream>

int main()
{
    //const char buffer[] = {'a', 'b', 'c', 'd'};
    const char buffer[] = {0x00, 0x01, 0x02, 0x03};
    
    uint32_t* num = (uint32_t*)(buffer);

    std::cout << std::hex << "Address Num : " << num << "\nAddress Buf : " << std::hex << (uint32_t*)(buffer+3) << "\nValue : " << *num << std::endl;
    if(num == (uint32_t*)(buffer))
    {
	    std::cout << "ture" << std::endl;
    }
    else
    {
	    std::cout << "false" << std::endl;
    }
    return 0;

}

#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main()
{
        char message[] =
"230-*************************************\r\n\
    **                                                       **\r\n\
    **             Garanti Bankasi AS         **\r\n\
230-**                                                       **\r\n\
    **                    FTP sitesi                   **\r\n\
    **                                                       **\r\n\
    *************************************\r\n";


        char* ptrbuf = 0;
        char* next_ptr = 0;
        char rx_buffer[1024] = {'\0', };

        ptrbuf = strtok_r(message, "\r\n", &next_ptr);

        while(ptrbuf)
        {
            if(isdigit(ptrbuf[0]) == 0 || isdigit(ptrbuf[1]) == 0 || isdigit(ptrbuf[2]) == 0)
            {
                /*
                do
                {
                    size_t rx_buffer_length = strlen(rx_buffer) + strlen("\r\n");
                    strncat(rx_buffer, "\r\n", strlen("\r\n"));
                    strncat(rx_buffer, ptrbuf, strlen(ptrbuf));
                    rx_buffer[rx_buffer_length + strlen(ptrbuf)] = '\0';
                    ptrbuf = strtok_r(next_ptr, "\r\n", &next_ptr);
                }
                while(ptrbuf);
                printf("parsing #1 rx_buffer = \n%s", rx_buffer);
                break;
                */
                ptrbuf = strtok_r(next_ptr, "\r\n", &next_ptr);
                continue;
            }
            else
            {
                strncpy(rx_buffer, ptrbuf, sizeof(rx_buffer) -1);
                rx_buffer[sizeof(rx_buffer) -1] = '\0';
                printf("parsing #2 %s\n", rx_buffer);
                ptrbuf = strtok_r(next_ptr, "\r\n", &next_ptr);
            }
        }
        return 0;
}

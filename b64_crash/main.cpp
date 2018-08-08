#include <cstdio>
#include <cstring>
#include <string>

const unsigned char base64_decode_mime[] =
{
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,  /* 00-0F */
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,  /* 100xffF */
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,  62,0xff,0xff,0xff,  63,  /* 20-2F */
    52,  53,  54,  55,  56,  57,  58,  59,  60,  61,0xff,0xff,0xff,0xff,0xff,0xff,    /* 30-3F */
    0xff,   0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,    /* 40-4F */
    15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,0xff,0xff,0xff,0xff,0xff,    /* 50-5F */
    0xff,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,    /* 60-6F */
    41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,0xff,0xff,0xff,0xff,0xff,    /* 70-7F */
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,  /* 80-8F */
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,  /* 90-9F */
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,  /* A0-AF */
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,  /* B0-BF */
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,  /* C0-CF */
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,  /* D0-DF */
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,  /* E0-EF */
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff   /* F0-FF */
};

static const char* message = "AQAFAH/lX4sBAAA9nn2iAAAAAAoAAdQZQVNdG9MBAAAAGAABAQEBEAIBAQMBFAcBAQgBAQoBAQsBCgAAACcAIQAeGjCaGUZYkThR0HYh1GphHh3QTrEuPSmRgbarh0Q6BAAACZ0A";

int decode_base64(char *org, char *dst, int len, char org_remaining[4])
{
    int i = 0;
    int j = 0;
    int m1, m2, m3, m4;

    memset(org_remaining, 0, 4);

    while (i < len)
    {
        while (base64_decode_mime[(int)org[i]] == 0xff)
        {
            i++;
        }
        if (i + 4 > len)
        {
            if (len > i)
            {
                memcpy(org_remaining, &org[i], len - i);
            }
            break;
        }
        m1 = base64_decode_mime[(int)org[i++]];
        m2 = base64_decode_mime[(int)org[i++]];
        m3 = base64_decode_mime[(int)org[i++]];
        m4 = base64_decode_mime[(int)org[i++]];
        dst[j++] = (m1 << 2) | ((m2 & 0x30) >> 4);
        dst[j++] = ((m2 & 0x0f) << 4) | ((m3 & 0x3c) >> 2);
        dst[j++] = ((m3 & 0x03) << 6) | m4;
        if (org[i-1] == '=')
        {
            j--;
            if (org[i-2] == '=')
            {
                j--;
            }
        }
    }

    return j;
}

template <size_t N, size_t M>
int decode_base64(char (&org)[N], char (&dst)[M], char org_remaining[4])
{
    int i = 0;
    int j = 0;
    int m1, m2, m3, m4;

    memset(org_remaining, 0, 4);

    while (i < N)
    {
        while (base64_decode_mime[(int)org[i]] == 0xff)
        {
            i++;
        }
        if (i + 4 > N)
        {
            if (N> i)
            {
                memcpy(org_remaining, &org[i], N- i);
            }
            break;
        }
        m1 = base64_decode_mime[(int)org[i++]];
        m2 = base64_decode_mime[(int)org[i++]];
        m3 = base64_decode_mime[(int)org[i++]];
        m4 = base64_decode_mime[(int)org[i++]];
        dst[j++] = (m1 << 2) | ((m2 & 0x30) >> 4);
        dst[j++] = ((m2 & 0x0f) << 4) | ((m3 & 0x3c) >> 2);
        dst[j++] = ((m3 & 0x03) << 6) | m4;
        if (org[i-1] == '=')
        {
            j--;
            if (org[i-2] == '=')
            {
                j--;
            }
        }
    }

    return j;
}

int main()
{
#if 1
    std::string key_management_data(message);
    unsigned char data[key_management_data.length()+1];
#else
    unsigned char data[strlen(message)+1];
#endif
    char remaining[4];
    int index = 0;
#if 0
    printf("key_management_data's length = %zu\n", key_management_data.length());
    index = decode_base64((char*)key_management_data.c_str(), (char*)data, sizeof(data), remaining);
#else
    printf("message's length = %zu\n", strlen((const char*)message));
#if 0
    index = decode_base64((char*)message, (char*)data, sizeof(data), remaining);
#else
    index = decode_base64((char [strlen(message)])message, data, remaining);
#endif
#endif

    data[index] = '\0';
    printf("data's length = %zu\n", strlen((const char*)data));
    printf("index = %d\n", index);

    for(int i = 0; i < index; i++)
    {
        printf("%2x ", data[i]);
        if(i % 4 == 3) printf("\r\n");
    }
    return 0;
}

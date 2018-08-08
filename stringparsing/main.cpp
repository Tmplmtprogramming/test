#include <cstdio>
#include <cstring>

int main()
{
    char temp_rxbuf[] =
#if 1
    "SET_PARAMETER rtsp://192.168.38.29:322/profile3/media.smp RTSP/1.0\r\nCSeq: 5\r\nSession: 30129\r\nUser-Agent: OmnicastRTSPClient/1.0\r\nContent-Type: text/parameters\r\nContent-Length: 145\r\nAuthorization: Digest username=\"admin\", realm=\"iPOLiS\", nonce=\"0CC1C2DD6630F5A6521B265991F5C7FE\", uri=\"rtsp://192.168.38.29:322/profile3/media.smp\", response=\"f28ab530f89420b5b7ac487a8521400b\"\r\n\r\nmikey: AQAFAEKecrkBAAAYe2JCAAAAAAoAAdQSrk1xzqQBAAAAGAABAQEBEAIBAQMBFAcBAQgBAQoBAQsBCgAAACcAIQAeAO+cOYyVUGqLFQplOiRGhXeX+vjNn5fCHa0TRLtOBAAAB/cA\r\n\r\n";
#else
    "SET_PARAMETER rtsp://192.168.38.29:322/profile3/media.smp RTSP/1.0\r\nCSeq: 5\r\nSession: 30129\r\nUser-Agent: OmnicastRTSPClient/1.0\r\nContent-Type: text/parameters\r\nContent-Length: 145\r\nAuthorization: Digest username=\"admin\", realm=\"iPOLiS\", nonce=\"0CC1C2DD6630F5A6521B265991F5C7FE\", uri=\"rtsp://192.168.38.29:322/profile3/media.smp\", response=\"f28ab530f89420b5b7ac487a8521400b\"\r\n\r\n";
#endif
    char* end = 0;

    char* temp_spot = (char*)temp_rxbuf;

    char* temp_spot_pre = temp_spot;
    size_t spot_count = 0;
    while(temp_spot != NULL)
    {
        temp_spot = strstr(temp_spot, "\r\n\r\n");
        if(temp_spot != NULL)
        {
            temp_spot_pre = temp_spot;
            temp_spot += strlen("\r\n\r\n");
            spot_count++;
            if(spot_count > 2)
            {
                break;
            }
        }
    }
    if(spot_count == 2)
    {
        *temp_spot_pre = '\0';
        printf("%s", temp_rxbuf);
        end = temp_spot_pre + strlen("\r\n\r\n");
    }
    else
    {
        //end = RtspRequestParser::split_str((char *) temp_rxbuf, "\r\n\r\n", 0);
        printf("%s\n", temp_rxbuf);
    }
}

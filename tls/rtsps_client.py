#!/usr/bin/python
# -*- coding: utf-8 -*-

from socket import *
from select import *

import sys
from time import ctime

import ssl

from time import sleep

host = "192.168.38.29"
port = 322 
#port =  554

BUFSIZE = 1024

ADDR = (host, port)

client_socket = socket(AF_INET, SOCK_STREAM)
client_socket.settimeout(10)

#$wrapped_socket = ssl.wrap_socket(client_socket, ca_certs="/etc/ssl/certs/ca-certificates.crt", cert_reqs=ssl.CERT_REQUIRED)
wrapped_socket = ssl.wrap_socket(client_socket)
#wrapped_socket = client_socket

try:
    wrapped_socket.connect(ADDR)

except Exception as e:
    print('%s:%s'%ADDR)
    sys.exit()
print('connect is success')
sleep(1)

option_message = "OPTIONS rtsp://192.168.38.29:322/profile2/media.smp RTSP/1.0\r\nCSeq: 2\r\nUser-Agent: LibVLC/2.2.6 (LIVE555 Streaming Media v2016.02.22)\r\n\r\n"
print("==========>\r\n{0}\r\n".format(option_message))
wrapped_socket.send(option_message)

print("<==========\r\n")
print(wrapped_socket.recv(1280));
sleep(1)

option_message_2 = "OPTIONS rtsp://192.168.38.29:322/profile2/media.smp RTSP/1.0\r\nCSeq: 3\r\nAuthorization: Digest username=\"admin\", realm=\"iPOLiS\", nonce=\"92AA8A906639D305823CE8A5A271B7F4\", uri=\"rtsp://192.168.38.29:322/profile2/media.smp\", response=\"52d8c048f899752474d9e053e74b3f00\"\r\nUser-Agent: LibVLC/2.2.6 (LIVE555 Streaming Media v2016.02.22)\r\n\r\n"
print("==========>\r\n{0}\r\n".format(option_message_2))
wrapped_socket.send(option_message_2);

print("<==========\r\n")
print(wrapped_socket.recv(1280));
sleep(1)

describe_message = "DESCRIBE rtsp://192.168.38.29:322/profile2/media.smp RTSP/1.0\r\nCSeq: 4\r\nAuthorization: Digest username=\"admin\", realm=\"iPOLiS\", nonce=\"92AA8A906639D305823CE8A5A271B7F4\", uri=\"rtsp://192.168.38.29:322/profile2/media.smp\", response=\"36383fafafbe495fa138b057ca18d3a1\"\r\nUser-Agent: LibVLC/2.2.6 (LIVE555 Streaming Media v2016.02.22)\r\nAccept: application/sdp\r\n\r\n"
print("==========>\r\n{0}\r\n".format(describe_message))
wrapped_socket.send(describe_message);

print("<==========\r\n")
print(wrapped_socket.recv(1280));
sleep(1)

#setup_message = "SETUP rtsp://192.168.38.29:322/profile2/media.smp/trackID=v RTSP/1.0\r\nCSeq: 5\r\nAuthorization: Digest username=\"admin\", realm=\"iPOLiS\", nonce=\"92AA8A906639D305823CE8A5A271B7F4\", uri=\"rtsp://192.168.38.29:322/profile2/media.smp\", response=\"4cab509698c5a60e4d2d22730a7efb59\"\r\nUser-Agent: LibVLC/2.2.6 (LIVE555 Streaming Media v2016.02.22)\r\nTransport: RTP/SAVP/TCP;unicast;interleaved=0-1\r\nKeyMgmt: prot=mikey;uri=\"\";data=\"AQAFAP1td9ABAADCD1UcAAAAAAoAAdOOGc75XD0BAAAAGAABAQEBEAIBAQMBFAcBAQgBAQoBAQsBCgAAACcAIQAe30C59UrClE0e27UP5h/Wty9UL8+dfzg+2ttmmo3kBAAAAC8A\"\r\n\r\n"
#setup_message = "SETUP rtsp://192.168.38.29:322/profile2/media.smp/trackID=v RTSP/1.0\r\nCSeq: 5\r\nAuthorization: Digest username=\"admin\", realm=\"iPOLiS\", nonce=\"92AA8A906639D305823CE8A5A271B7F4\", uri=\"rtsp://192.168.38.29:322/profile2/media.smp\", response=\"4cab509698c5a60e4d2d22730a7efb59\"\r\nUser-Agent: LibVLC/2.2.6 (LIVE555 Streaming Media v2016.02.22)\r\nTransport: RTP/SAVP/UDP;unicast;client_port=40048-40049\r\nKeyMgmt: prot=mikey;uri=\"\";data=\"AQAFAP1td9ABAADCD1UcAAAAAAoAAdOOGc75XD0BAAAAGAABAQEBEAIBAQMBFAcBAQgBAQoBAQsBCgAAACcAIQAe30C59UrClE0e27UP5h/Wty9UL8+dfzg+2ttmmo3kBAAAAC8A\"\r\n\r\n"
setup_message = "SETUP rtsp://192.168.38.29:322/profile2/media.smp/trackID=v RTSP/1.0\r\nCSeq: 5\r\nAuthorization: Digest username=\"admin\", realm=\"iPOLiS\", nonce=\"92AA8A906639D305823CE8A5A271B7F4\", uri=\"rtsp://192.168.38.29:322/profile2/media.smp\", response=\"4cab509698c5a60e4d2d22730a7efb59\"\r\nUser-Agent: LibVLC/2.2.6 (LIVE555 Streaming Media v2016.02.22)\r\nTransport: RTP/SAVP;unicast;client_port=15000-15001\r\nKeyMgmt: prot=mikey;uri=\"\";data=\"AQAFAP1td9ABAADCD1UcAAAAAAoAAdOOGc75XD0BAAAAGAABAQEBEAIBAQMBFAcBAQgBAQoBAQsBCgAAACcAIQAe30C59UrClE0e27UP5h/Wty9UL8+dfzg+2ttmmo3kBAAAAC8A\"\r\n\r\n"
print("==========>\r\n{0}\r\n".format(setup_message))
wrapped_socket.send(setup_message)

print("<==========\r\n")
print wrapped_socket.recv(1280)
sleep(1)

play_message = "PLAY rtsp://192.168.38.29:322/H.264/media.smp RTSP/1.0\r\nCSeq: 6\r\nSession: null\r\nUser-Agent: UWC[192.168.38.40]\r\n\r\n"
print("==========>\r\n{0}\r\n".format(play_message))
wrapped_socket.send(play_message)

print("<==========\r\n")
print wrapped_socket.recv(1280)
sleep(5)

teardown_message = "TEARDOWN rtsp://192.168.38.29:322/profile2/media.smp RTSP/1.0\r\nCSeq: 8\r\nUser-Agent: LibVLC/2.2.6 (LIVE555 Streaming Media v2016.02.22)\r\nSession: 62130\r\n\r\n"
print("==========>\r\n{0}\r\n".format(teardown_message))
wrapped_socket.send(teardown_message)
print("<==========\r\n")
print(wrapped_socket.recv(1280));
sleep(1)
wrapped_socket.close()

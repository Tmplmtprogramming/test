#!/usr/bin/python
# -*- coding: utf-8 -*-

from socket import *
from select import *

import sys
from time import ctime

import ssl

import subprocess
from time import sleep

host = "192.168.38.29"
#port = 332 
port =  80

BUFSIZE = 1024

if len(sys.argv) == 1:
    i = 1
#elif not isNumber(sys.argv[1]):
#    print('Input Number for the count')
#    exit(2)
else:
    i = int(sys.argv[1])
while i > 0:
    option_message = 'curl -v --digest -u admin:5tkatjd!! --data-urlencode @ssl.xml "http://192.168.38.29/stw-cgi/factory.cgi?InsertDeviceCert=True&macAddr=00:16:6C:F9:27:BF" -H "Expect:"'
    subprocess.Popen(option_message, shell=True);
    sleep(2)
    print("################### COUNT = {0}".format(1001-i))
    sleep(3)
    i -= 1

print('##############################################################\nEnd of process')

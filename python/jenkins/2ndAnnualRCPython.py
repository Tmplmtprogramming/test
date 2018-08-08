#!/usr/bin/python
# -*- coding: utf-8 -*-
import subprocess
from time import sleep

# Get User info
print('*************************')
print('Jenkins remote python script')
print('*************************')

User = str(raw_input("Username(teamforge):"))
Passwd = str(raw_input('Password:'))
choice = str(raw_input('[I]mmediately or [R]eservation ? '))

#job_list=['WN5_2M_Box_Bullet', 'WN5_5M_Box_Bullet', 'XNB-6005_XNO-6085', 'WN5_12x_Bullet_Vandal', 'WN5_2M_Compact_Bullet', 'WN5_2M_Compact_Dome_Vandal', 'WN5_2M_Compact_Dome_Vandal_IR', 'WN5_2M_Dome_Vandal_IR', 'WN5_5M_Compact_Bullet', 'WN5_5M_Compact_Dome_Vandal_IR', 'WN5_5M_Dome_Vandal_IR', 'WN5_5M_IR_Fisheye', 'XNB-6001', 'XND-6011F_8020F', 'XNP-6040H', 'XNP-6120H', 'XNV-6011', 'XNV-6085_XND-6085_6085V', 'TNB-6030']
job_list=['WN5_2M_Box_Bullet']

build_parameter='Deploy=Development&QualityReport=false&REV=HEAD&FW_VERSION=9.78'
#build_parameter='Deploy=Stable&QualityReport=true&MAKESPARE=true&FORCE_UPGRADE_ALL=true&FW_VERSION=1.14&REV=HEAD'

def run_CI(list):
    jobCount = 0
    message = []
    for jobname in list:
        message.insert(jobCount, str('http://'+ User + ':' + Passwd + '@build-se.net/job/' + jobname + '/buildWithParameters?' + build_parameter))
        print(message[jobCount])
        jobCount = jobCount + 1
    print("Total Job Count : {0}".format(jobCount))
    for jobname in list:
        jobCount = jobCount -1
        if choice == 'i' or choice == 'I':
            while jobCount >= 0:
                command = 'curl -X POST ' + str(message[jobCount])
                print(str(command))
                subprocess.Popen(command, shell=True)
                jobCount = jobCount - 1
        elif choice == 'R' or choice == 'r':       
            print("Not yet implemented");
        sleep(1)

run_CI(job_list)

#jobCount = 0
#message = []
#print("Total Job Count : {0}".format(jobCount))
#jobCount = jobCount -1
#if choice == 'i' or choice == 'I':
#    while jobCount >= 0:
#        command = 'curl -X POST ' + str(message[jobCount])
#        print(str(command))
#        subprocess.Popen(command, shell=True)
#        jobCount = jobCount - 1
#elif choice == 'R' or choice == 'r':       
#    print("Not yet implemented");

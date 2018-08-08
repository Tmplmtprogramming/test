#!/usr/bin/python
# -*- coding: utf-8 -*-
import subprocess

# Get User info
print('*************************')
print('Jenkins remote python script')
print('*************************')

User = str(raw_input("Username(teamforge):"))
Passwd = str(raw_input('Password:'))
choice = str(raw_input('[I]mmediately or [R]eservation ? '))

job_list = ['WN5_2M_Box_Bullet_RC', 'WN5_5M_Box_Bullet_RC', 'XNB-6005_XNO-6085_RC', 'WN5_12x_Bullet_Vandal_RC', 'WN5_2M_Compact_Bullet_RC', 'WN5_2M_Compact_Dome_Vandal_RC', 'WN5_2M_Compact_Dome_Vandal_IR_RC', 'WN5_2M_Dome_Vandal_IR_RC', 'WN5_5M_Compact_Bullet_RC', 'WN5_5M_Compact_Dome_Vandal_IR_RC', 'WN5_5M_Dome_Vandal_IR_RC', 'WN5_5M_IR_Fisheye_RC', 'XNB-6001_RC', 'XND-6011F_8020F_RC', 'XNP-6040H_RC', 'XNP-6120H_RC', 'XNV-6011_RC', 'XNV-6085_XND-6085_6085V_RC', 'TNB-6030_RC']
#job_list = ['WN5_2M_Box_Bullet_RC']

#build_parameter = 'Deploy=Development&QualityReport=false&wn5=HEAD'
build_parameter ='Deploy=Stable&QualityReport=true&MAKESPARE=true&FORCE_UPGRADE_ALL=true&FW_VERSION=1.14&wn5=HEAD'

jobCount = 0

message = []

for jobname in job_list:
    message.insert(jobCount, str('http://'+ User + ':' + Passwd + '@build-se.net/job/' + jobname + '/buildWithParameters?' + build_parameter))
    print(message[jobCount])
    jobCount = jobCount + 1

print("Total Job Count : {0}".format(jobCount))
jobCount = jobCount -1
if choice == 'i' or choice == 'I':
    while jobCount >= 0:
        command = 'curl -X POST ' + str(message[jobCount])
#        print(str(command))
        subprocess.Popen(command, shell=True)
        jobCount = jobCount - 1
elif choice == 'R' or choice == 'r':       
    print("Not yet implemented");

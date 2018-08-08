#!/bin/bash


# Check & Install 'at' package
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' at | grep "install ok installed")
echo Checking for at : $PKG_OK
if [ "" == "$PKG_OK" ]; then
  echo "No at package. Setting up at."
  sudo apt-get --force-yes --yes install at
fi

# Get User info
echo '*************************'
echo 'Jenkins remote API script'
echo '*************************'
read -p 'Username(teamforge): ' USER
read -sp 'PassWord: ' PASSWD
echo ''
read -p '[I]mmediately or [R]eservation ? '  choice

# Jenkins job list 
#job_list="WN5_2M_Box_Bullet"
job_list=" TNO-4040_4050 X_Series_2M_Car_NWCAM XNP-6371RH XNP-6320_H_HS WN5_X_Lite_6 WN5_5M_IR_Fisheye XNB-6005_XNO-6085 XND-6011F_8020F XNP-6040H XNV-6011 XNB-6001 XNP-6120H WN5_12x_Bullet_Vandal XNV-6085_XND-6085_6085V TNB-6030 WN5_5M_Compact_Dome_Vandal_IR WN5_5M_Compact_Bullet WN5_2M_Compact_Dome_Vandal_IR WN5_2M_Compact_Bullet WN5_5M_Dome_Vandal_IR WN5_5M_Box_Bullet WN5_2M_Dome_Vandal_IR WN5_2M_Compact_Dome_Vandal WN5_2M_Box_Bullet"
#job_list="WN5_X_Lite_6 XNP-6320_H_HS XNP-6371RH X_Series_2M_Car_NWCAM TNO-4040_4050"

# Build Parameter
#BuildParameter='Deploy=Development&QualityReport=false&REV=HEAD&FW_VERSION=9.78'
BuildParameter='Deploy=Stable&QualityReport=false&MAKESPARE=true&FORCE_UPGRADE_ALL=true&FW_VERSION=2.00&REV=HEAD'


jobCount=0
for jobname in `echo $job_list`
do
	((jobCount++));
	message[$jobCount]="http://${USER}:${PASSWD}@build-se.net/job/${jobname}/buildWithParameters?${BuildParameter}"
	echo ${message[$jobCount]}
done
echo "$jobCount post message created"

case "$choice" in
 R|r )
	read -p 'Enter Time (00:00) ' rTime
	while [ $jobCount -gt 0 ]
	do
		#echo "curl -X POST ${message[$jobCount]} > /dev/null" | at $rTime > /dev/null 2>&1
		echo "curl -X POST ${message[$jobCount]} > /dev/null" | at now + 1 min
		((jobCount--));
	done
	atq
 ;;
 I|i )
	while [ $jobCount -gt 0 ]
	do
		curl -X POST ${message[$jobCount]}
		((jobCount--));
        sleep 60
	done
 ;;
esac
echo "done"


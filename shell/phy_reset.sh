#!/bin/sh
sleep 1
rx_check_duration=120
arp_duration=5
rx_count=0
zero=0
old_rx_count=0
new_rx_count=0
while [ $zero -eq 0 ]
do
#    echo "arp check!"
    arping -I eth0 -c 1 `route -n | awk '{if($4=="UG")print $2}'`
    if [ $rx_count -ge $rx_check_duration ]; then
        old_rx_count=$new_rx_count
        new_rx_count=`awk '/count/ {print $2}' /proc/irq/45/spurious`
        echo $new_rx_count
        echo $old_rx_count
        if [ $new_rx_count -le $old_rx_count ]; then
            echo "RX count is NOT increased !!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
            echo "-------------------- PHY reset now --------------------"
            mii-diag -R
        fi
        rx_count=0
    fi
    rx_count=$(expr "$rx_count" + "$arp_duration")
    echo "rx_count = $rx_count"
    sleep $arp_duration
done

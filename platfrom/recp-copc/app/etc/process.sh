#!/bin/bash

#APPS+=("app_name -- opthin" )
#APPS=("/usr/sbin/raced " )
#APPS+=("/usr/sbin/fand " )
#APPS+=("/sbin/mosquitto -- -c /etc/mosquitto/mosquitto.conf -d " )
#APPS+=("/usr/bin/led_ctl " )
#APPS+=("/bin/rpc_server " )
APPS+=("/bin/rpc_client " )
APPS+=("/bin/system_info_server ")
APPS+=("/usr/bin/ca_server " )

app_start()
{
    for i in "${!APPS[@]}"
    do
        start-stop-daemon -K -t --exec ${APPS[$i]} 2>&1 >/dev/null
    done
}

check_app_and_up()
{
    for i in "${!APPS[@]}"
    do
        start-stop-daemon -K -t --exec ${APPS[$i]} 2>&1 >/dev/null
        [ "$?" -eq "0" ] && continue
        logger "error process stop ${APPS[$i]}"
        start-stop-daemon --start  --background --exec ${APPS[$i]}

    done
}

app_start

while true 
do 
    check_app_and_up 
    sleep 30
done


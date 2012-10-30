#!/bin/sh

#command to find ttl
#abc="$(ping 192.16.126.65 -c 10 | grep 'time=' | awk '{ print $7}' | cut -d '=' -f2)"

#IP_ADDRESS=192.16.126.65 

TTL_STRING="$(ping $1 -i .2 -c 1 | grep 'rtt' | awk '{ print $4}')"

TTL=$(echo $TTL_STRING | cut -d '/' -f2)

echo $TTL



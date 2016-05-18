#!/bin/bash

sudo git pull
sudo sleep 2

sudo make
./InetS3 1337
sudo ps -el | grep Inet
sudo sleep 10
sudo ps -el | grep Inet
cat /var/log/syslog |grep Inet
sleep 30
sudo kill -15 $(pgrep Inet)
sudo ps -el | grep Inet
cat /var/log/syslog |grep Inet
cat /var/log/syslog |grep HEARTS

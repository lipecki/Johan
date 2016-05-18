#!/bin/bash

./InetS3 1337
sudo ps -el | grep Inet
sudo sleep 10
sudo ps -el | grep Inet
cat /var/log/syslog
sudo sleep 2
sudo ps -el | grep Inet
cat /var/log/syslog
sudo sleep 1
sudo ps -el | grep Inet
cat /var/log/syslog
sudo sleep 1
sudo ps -el | grep Inet
cat /var/log/syslog
sudo sleep 1
sudo ps -el | grep Inet
cat /var/log/syslog
sudo sleep 1
sudo ps -el | grep Inet
cat /var/log/syslog
sudo sleep 2
sudo ps -el | grep Inet
cat /var/log/syslog
sudo sleep 2
sudo ps -el | grep Inet
cat /var/log/syslog
sudo sleep 2
sudo ps -el | grep Inet


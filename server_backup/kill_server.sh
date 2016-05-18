#!/bin/sh
echo $(pgrep server)
sudo kill -15 $(pgrep server)
echo $(pgrep server)
cat /var/log/syslog | grep server

#!/bin/sh
echo $(pgrep Inet)
sudo kill -15 $(pgrep Inet)
echo $(pgrep Inet)

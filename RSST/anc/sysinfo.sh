#!/usr/bin/env bash
gcc -v >& gcc-v.txt
uname -a > uname-a.txt
lscpu > lscpu.txt
sudo lshw -short > lshw.txt

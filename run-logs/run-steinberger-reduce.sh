#!/usr/bin/env bash
gcc -v  > gcc-v.txt 2>&1
gcc anc/reduce.c > gcc-reduce-log.txt 2>&1
time ./a.out anc/U_2822.conf > run-log.txt 2>&1

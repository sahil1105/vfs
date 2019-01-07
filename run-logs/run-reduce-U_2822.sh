#!/usr/bin/env bash
cd ..
gcc -v RSST/anc/reduce.c >& gcc-log.txts
time RSST/anc/a.out RSST/anc/U_2822.conf >& U_2822-log.txt


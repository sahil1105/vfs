#!/usr/bin/env python 
import sys
import pprint as pp

def map_int(L):
    return [int(e) for e in L]

def conf_parse_error(C, X):
    print('*** ERROR ***: parsed', len(C), 'configurations:')
    pp.pprint(C)
    print('.. but now *** CONFUSED *** about:', X)
    raise ValueError(X)

# dictionary of configurations
CONF = {}  

with open(sys.argv[1], 'r') as f:
    for line in f:
        L = line.split()
        if L != []:  
            A, *rest = L                # What attribute are we looking at? Everything else in rest .. 
            if A == 'C':   
                ID = int(L[1])          # We have a new config here! ID defined now.. 
                CONF[ID] = {}           # Create empty dictionary for this config..
                status = f.readline()   # Next line after "C <NNN>" has reducibility status: R, I, or U
                CONF[ID]['S'] = status[0] # 'S' is for status
                if not status[0] in ['R','I','U']:
                    conf_parse_error(CONF, status)
            elif A in ['V','M']:
                CONF[ID][A] = int(rest[0])   # vertex count C and mysterious M
            elif A in ['T','HI','AB']:
                CONF[ID][A] = map_int(rest)  # int list of all stuff after A
            elif A == 'J':
                CONF[ID][A] = rest      # list of all stuff after A
            elif A.isdigit():           # adjacency list; we've read 'V' before!
                CONF[ID]['N'] = []      # create a new adjacency (i.e., *N*eighbors) list
                CONF[ID]['N'].append(map_int(L))   # .. and add first member
                for i in range(CONF[ID]['V'] - 1):   # .. now for the other members
                    CONF[ID]['N'].append(map_int(f.readline().split()))
            elif A == 'U':
                CONF[ID]['U'] = True
            else:
                conf_parse_error(CONF, L)

print('# Read %d configurations. Here they are:' % len(CONF))           
pp.pprint(CONF)

                
                
            

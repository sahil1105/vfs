#!/usr/bin/env python 
import sys
import numpy as np

epsilon = 1e-6  # low precision input (1e-9 misses edges)

points = np.loadtxt(sys.argv[1], delimiter=',', usecols=(1,2))

N = len(points)

edges = []
for i in range(N):
    for j in range(i+1,N):
        dist = sum( (points[i] - points[j])**2 )
        if abs(dist - 1.0) < epsilon:
            edges.append( (i+1,j+1) )
            
print(edges)
print("Found", len(edges), "edges of unit length.")

#!/usr/bin/env python 
import sys
epsilon = 1e-6  # low precision input (1e-9 misses edges)

points = [] 

with open(sys.argv[1], 'r') as f:
    for line in f:
        ID, x, y = line.split(",")
        points.append( (float(x), float(y)) )

N = len(points)

edges = []
for i in range(N):
    for j in range(i+1,N):
        dist = ( points[i][0] - points[j][0] )**2 + ( points[i][1] - points[j][1] )**2
        if abs(dist - 1.0) < epsilon:
            edges.append( (i+1,j+1) )

print(edges)
print("Found", len(edges), "edges of unit length.")

#!/usr/bin/python3

import random
import math
import numpy as np

octaves = 20
gain = 0.8
ZOOM = 1
amplitude_y =  0.0003 * ZOOM
z = 0

# for x in range(octaves):
#     num = random.random()
#     num = num * 2 * math.pi
#     print(f"vec2({math.cos(num):.4f}, {math.sin(num):.4f}),")

# for x in range(octaves):
#     z = z + pow(gain, x)
# 
# print(z * amplitude_y)

a1 = 0.89443 # sin(arctan(2))
a2 = 0.44721 # cos(arctan(2))
a3 = 0.86603 # cos(arcsin(0.5)) = sin(arccos(0.5))

A = np.array([[ a1, a1, 0],
              [-a2, a2, a3],
              [-a3, a3, 0.5]])
 
b1 = 0.86603 # cos(30)
b2 = 0.5     # sin(30)
b3 = 0.57735 # sin(30)/cos(30)
# WorldPosToCartesian
B = np.array([[  b1, b1],
              [ -b2, b2]])

# CartesianToWorldPos
Binv = np.array([[  b3, -1.0],
                 [  b3,  1.0]])

# Center to vertec
c1 = 0.5/math.cos(30*math.pi/180)
# Center to neigh center in cartes x
c2 = math.cos(30*math.pi/180)

vec = np.array([[c1],
                [0.0]])
vec2 = np.array([[2*c1],
                 [0.0]])

print(f"From {c1} to {2*c2 - c1}" )
print(f"Len {2*c2 - 2*c1}" )

# Calculating the inverse of the matrix
print(np.linalg.inv(B))
print(f"x, y: {np.matmul(Binv, vec)}")
print(f"x, y: {np.matmul(Binv, vec2)}")

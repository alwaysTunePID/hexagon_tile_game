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
 
# Calculating the inverse of the matrix
# np.matmul(np.linalg.inv(A), A)
print(np.linalg.inv(A))

#!/usr/bin/env python
import ctypes
import os
import subprocess
import sys
import timeit
from ctypes import byref, c_char, c_float, c_int
from ctypes.util import find_library

import numpy

libblas_so_name = sys.argv[1] if len(sys.argv) > 1 else find_library('blas')
_blaslib = ctypes.cdll.LoadLibrary(libblas_so_name or 'libblas.so')

def Mul(m1, m2, i, r):

	no_trans = c_char(b"n")
	n = c_int(i)
	one = c_float(1.0)
	zero = c_float(0.0)

	_blaslib.sgemm_(byref(no_trans), byref(no_trans), byref(n), byref(n), byref(n), byref(one), m1.ctypes.data_as(ctypes.c_void_p), byref(n),
																								m2.ctypes.data_as(ctypes.c_void_p), byref(n), byref(zero),
																								r.ctypes.data_as(ctypes.c_void_p), byref(n))

if __name__ == '__main__':

	rNumpy = []
	rBlas = []

	p = subprocess.Popen("./bench {0}".format(str([x for x in range(5, 501, 5)])[1:-1]), shell = True)
	os.waitpid(p.pid, 0)

	for i in range(20, 501, 20):
		m1 = numpy.random.rand(i,i).astype(numpy.float32)
		m2 = numpy.random.rand(i,i).astype(numpy.float32)

		tNumpy = timeit.Timer("numpy.dot(m1, m2)", "import numpy; from __main__ import m1, m2")
		rNumpy.append((i, tNumpy.repeat(20, 1)))

		r = numpy.zeros((i,i), numpy.float32)
		tBlas = timeit.Timer("Mul(m1, m2, i, r)", "import numpy; from __main__ import i, m1, m2, r, Mul")
		rBlas.append((i, tBlas.repeat(20, 1)))
		print(i)

	f = open("Test.csv", "w")

	for (i, n), (j, b) in zip(rNumpy, rBlas):
		f.write("{0} {1} {2} {3} ".format(i, sum(n)/len(n), min(n), max(n)))
		f.write("{0} {1} {2} {3}\n".format(j, sum(b)/len(b), min(b), max(b)))

	f.close()

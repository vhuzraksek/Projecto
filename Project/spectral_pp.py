import argparse
import sys
import numpy as np
import pandas as pd
from myspectral import spectral
from kmeans_pp import kmeans_main

def print_output(centroids, indices):
	print(*indices, sep=",")
	for centroid in centroids:
		print(*[np.float64(axis) for axis in centroid], sep=",")

def spectral_mapping_to_points(spectral_mapping):
	pass

def spectral_main(sample_size, dims, points):
	ret = spectral(sample_size, dims, points)
	if ret[-1] == -1:
		sys.exit("Execution of Normalized Spectral Clustering failed due to bad malloc")
	spectral_mapping, k = ret[0], ret[1]
	return (spectral_mapping, k)
	# results = kmeans_main(k, spectral_points)
	# print_output(centroids, indices)
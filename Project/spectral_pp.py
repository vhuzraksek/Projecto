import math
import argparse
import sys
import numpy as np
import pandas as pd
from myspectral import spectral
from kmeans_pp import kmeans_main

def parse_k(aHatMat):
	# Parsing aHatMat returned from the C implementation
	# to extract the argmax of the eigenvalues
	eigenvalues = []
	for i in range(len(aHatMat)):
		eigenvalues.append(aHatMat[i][i])
	eigenvalues = sorted(eigenvalues)
	maxdiff = 0
	argmax = 1
	for i in range((len(eigenvalues) // 2)):
		diff = abs(eigenvalues[i] - eigenvalues[i+1])
		if (diff > maxdiff):
			maxdiff = diff
			argmax = i + 1
	return argmax

def parse_points(qHatMat, k):
	# Parsing the qHatMat returned from the C implementation
	# to extract the eigenvectors to run kmeans on
	newvectors = np.array(qHatMat)[::, :k]
	for i in range(len(newvectors)):
		norm = math.sqrt(sum([element ** 2 for element in newvectors[i]]))
		if norm == 0:
			continue
		for j in range(len(newvectors[i])):
			newvectors[i][j] = newvectors[i][j] / norm
	return newvectors, qHatMat
	
def spectral_main(sample_size, dims, points):
	ret = spectral(sample_size, dims, points)
	if ret[-1] == -1:
		sys.exit("Execution of Normalized Spectral Clustering failed due to bad malloc")
	eigenvectors, eigenvalues = ret[0], ret[1]

	k = parse_k(eigenvalues)
	spectral_points, _ = parse_points(eigenvectors, k)
	spectral_mapping = kmeans_main(k, spectral_points.tolist())
	return (spectral_mapping, k)
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
	eigenvectors = np.array(qHatMat[:k]).transpose()
	for i in range(len(eigenvectors)):
		norm = math.sqrt(sum([element ** 2 for element in eigenvectors[i]]))
		for j in range(len(eigenvectors[i])):
			eigenvectors[i][j] = eigenvectors[i][j] / norm
	return eigenvectors
	
def spectral_main(sample_size, dims, points):
	ret = spectral(sample_size, dims, points)
	if ret[-1] == -1:
		sys.exit("Execution of Normalized Spectral Clustering failed due to bad malloc")
	# eigenvectors, eigenvalues, q , r , nor = ret[0], ret[1], ret[2], ret[3] , ret[4]
	# print("q={} \n, r={} \n, nor={} \n".format(q,r,nor))
	eigenvectors, eigenvalues = ret[0], ret[1]

	k = parse_k(eigenvalues)
	spectral_points = parse_points(eigenvectors, k)
	spectral_mapping = kmeans_main(k, spectral_points.tolist())
	print(k)
	return (spectral_mapping, k)
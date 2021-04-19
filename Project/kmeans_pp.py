import argparse
import numpy as np
import pandas as pd
from mykmeanssp import kmeans

MAX_ITER = 300

def init_centroids(points, K):
	# Main logic of the function
	indices = []
	centroids = []
	centroid, idx = create_random_centroid(points)
	centroids.append(centroid)
	indices.append(idx)
	for j in range(K - 1):
		centroid, idx = create_next_centroid(centroids, points)
		centroids.append(centroid)
		indices.append(idx)
	return centroids, indices

def create_random_centroid(points, probs=None):
	# Add random point to centroid list using weighted choice
	idx = np.random.choice(len(points), 1, p=probs)
	return points[idx[0]], idx[0]

def calc_dist_squared(vecA, vecB):
	assert len(vecA) == len(vecB), "Something went wrong, tried calculating distance of two non-equal dimensions vectors."
	summ = 0
	for idx in range(len(vecA)):
		summ += ((vecA[idx] - vecB[idx]) ** 2)
	return summ

def build_dist_list(centroids, points):
	# Build distance list - each points will be mapped to the distance to the closest centroid
	min_dists = []
	for point in points:
		dists = [calc_dist_squared(centroid, point) for centroid in centroids]
		min_dists.append(min(dists))
	return min_dists

def build_prob_list(dists):
	# Build prob list of given centroids
	dist_sum = sum(dists)
	p = [dist / dist_sum for dist in dists]
	return p

def create_next_centroid(centroids, points):
	# Create another centroid based on the previous one using weighted choice
	dists = build_dist_list(centroids, points)
	probs = build_prob_list(dists)
	return create_random_centroid(points, probs)

def print_output(centroids, indices):
	print(*indices, sep=",")
	for centroid in centroids:
		print(*[np.float64(axis) for axis in centroid], sep=",")

def kmeans_main(K, points):
	centroids, indices = init_centroids(points, K)
	centroids = kmeans(MAX_ITER, points, centroids)
	print_output(centroids, indices)
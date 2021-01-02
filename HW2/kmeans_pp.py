import argparse
import numpy as np
import pandas as pd
from mykmeanssp import kmeans

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


def get_args():
	# Fetch arguments from commandline
	parser = argparse.ArgumentParser()
	parser.add_argument("K", help="the number of clusters required", type=int)
	parser.add_argument("N", help="the number of observations in the file", type=int)
	parser.add_argument("d", help="the dimension of each observation and initial centroids", type=int)
	parser.add_argument("MAX_ITER", help="the maximum number of iterations of the K-means algorithm", type=int)
	parser.add_argument("CSV_PATH", help="the path to the csv file containing the arguments", type=str)
	args = parser.parse_args()
	assert args.K < args.N, "K must be smaller than N"
	return args

def parse_args():
	# Using argparse to read the arguments
	args = get_args()
	points = parse_csv(args.CSV_PATH, args.d, args.N)
	return args.K, args.MAX_ITER, points

def parse_csv(csv_path, dims, obs_num):
	# Using panda to read the CSV
	df = pd.read_csv(csv_path, header=None)
	assert len(df) == obs_num, "Input file had wrong number of observations"
	assert len(df.columns) == dims, "Input file had wrong number of dimensions per point"
	return df.to_numpy().tolist()

def print_output(centroids, indices):
	print(*indices, sep=",")
	for centroid in centroids:
		print(*[np.float64(axis) for axis in centroid], sep=",")

def main():
	np.random.seed(0)
	K, MAX_ITER, points = parse_args()
	centroids, indices = init_centroids(points, K)
	centroids = kmeans(MAX_ITER, points, centroids)
	print_output(centroids, indices)


if __name__ == "__main__":
	main()
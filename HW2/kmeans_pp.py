import argparse
import numpy as np
import pandas as pd

def init_centroids(points, K):
	# Main logic of the function
	centroids = []
	centroids.append(create_random_centroid(points))
	for j in range(K - 1):
		centroids.append(create_next_centroid(centroids, points))
	return centroids

def create_random_centroid(points, probs=[]):
	# Add random point to centroid list using weighted choice
	idx = np.random.choice(len(points), 1, probs)
	return points[idx]

def build_dist_list(centroids, points):
	# Build distance list - each points will be mapped to the distance to the closest centroid
	min_dists = []
	for point in points:
		dists = np.array([np.linalg.norm(point - centroid, 2) for centroid in centroids])
		min_dists.append(dists[np.argmin(dists)])
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
	return df.to_numpy()

def pass_data(centroids, points, MAX_ITER):
	# Use this method to pass on the data to the C extensions
	print(centroids)
	pass

def main():
	np.random.seed(0)
	K, MAX_ITER, points = parse_args()
	centroids = init_centroids(points, K)
	pass_data(centroids, points, MAX_ITER)


if __name__ == "__main__":
	main()
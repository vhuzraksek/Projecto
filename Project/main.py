import kmeans_pp
import argparse
import sklearn.datasets

def get_args():
	# Fetch arguments from commandline
	parser = argparse.ArgumentParser()
	parser.add_argument("k", help="the number of clusters required", type=int)
	parser.add_argument("n", help="the number of observations in the file", type=int)
	parser.add_argument("random", help="the number of observations in the file", type=bool)
	args = parser.parse_args()
	assert args.k < args.n, "K must be smaller than N"
	return args

def generate_blobs(n, k, random):
	# Generate the random blobs of data and write them to a file
	pass

def print_results(results):
	# Print the results to console and write them to a file
	pass

def visualize_results(results):
	# Create pdf of results
	pass

def handle_results(k, spectral_results, kmeans_results):
	# Handle both results and supply demanded output of run
	print_results(k, spectral_results, kmeans_results)
	visualize_results(k, spectral_results, kmeans_results)
	
def main():
	args = get_args()
	print(args.k, args.n, args.random)
	points, trash, centers = sklearn.datasets.make_blobs(n_samples=10, n_features=3, centers=3, return_centers=True)
	print(points)
	print(centers)
	print(kmeans_pp)
	kmeans_results = kmeans_pp.kmeans_main(args.k, points.tolist())

if __name__ == "__main__":
	main()
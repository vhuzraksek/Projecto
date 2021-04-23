import sklearn.datasets
import numpy as np
import spectral_pp
import kmeans_pp
import argparse
from matplotlib.backends.backend_pdf import PdfPages
import matplotlib.pyplot as plt
import itertools

MAX_K_DICT = {2:3, 3:3}
MAX_N_DICT = {2:10, 3:10}

def get_args():
	# Fetch arguments from commandline
	parser = argparse.ArgumentParser()
	parser.add_argument("k", help="the number of clusters required", type=int)
	parser.add_argument("n", help="the number of observations in the file", type=int)
	parser.add_argument("random", help="is random", type=str)
	args = parser.parse_args()
	assert args.k < args.n, "K must be smaller than N"
	args.random = (args.random=="True")
	return args

def generate_blobs(n, k, random):
	# Generate the random blobs of data and write them to a file
	dims = np.random.randint(2,4)
	if random:
		k = np.random.randint(2, MAX_K_DICT[dims])
		n = np.random.randint(k, MAX_N_DICT[dims])

	points, _, centers = sklearn.datasets.make_blobs(n_samples=n, n_features=dims, centers=k, return_centers=True)
	### TODO: WRITE TO FILE
	return points, dims

def visualize_results(points, clusters, k, dims):
	projection = '3d'
	if dims == 2:
		projection = None

	fig = plt.figure()
	ax = fig.add_subplot(projection=projection)
	cmap = plt.cm.get_cmap('hsv', k + 1)
	for (idx, point) in enumerate(points):
		ax.scatter(*point, color=cmap(clusters[idx]))
	pp = PdfPages('clusters.pdf')
	pp.savefig(fig)
	pp.close()

def handle_results(k, spectral_results, kmeans_results):
	# Handle both results and supply demanded output of run
	print_results(k, spectral_results, kmeans_results)
	visualize_results(k, spectral_results, kmeans_results)
	
def main():
	args = get_args()
	points, dims = generate_blobs(args.n, args.k, args.random)
	spectral_results, infered_k = spectral_pp.spectral_main(args.n, dims, points.tolist())
	# visualize_results(points, spectral_results, 3, dims)


	# We'll be using the K yielded from spectral rather than the argument given for both spectral and kmeans.
	infered_k = args.k
	centroids, mapping = kmeans_pp.kmeans_main(infered_k, points.tolist())
	visualize_results(points, mapping, infered_k, dims)

if __name__ == "__main__":
	main()
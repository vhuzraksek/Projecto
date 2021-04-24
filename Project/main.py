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
	if (args.k > args.n):
		sys.exit("K must be smaller than N")
	args.random = (args.random=="True")
	return args

def generate_blobs(n, k, random):
	# Generate the random blobs of data and write them to a file
	dims = np.random.randint(2,4)
	if random:
		k = np.random.randint(2, MAX_K_DICT[dims])
		n = np.random.randint(k, MAX_N_DICT[dims])
	points, mapping = sklearn.datasets.make_blobs(n_samples=n, n_features=dims, centers=k)
	with open('data.txt', 'w') as f:
		for (point, mapped) in zip(points, mapping):
			f.write(','.join([str(axis) for axis in point]) + ',' + str(mapped) + '\n')

	return points, mapping, dims

def calc_jac(orig_mapping, calced_mapping):
	s1 = set(orig_mapping)
	s2 = set(calced_mapping)
	return float(len(s1.intersection(s2)) / len(s1.union(s2)))	

def visualize_results(points, clusters, k, dims, fig, idx, title):
	projection = '3d'
	if dims == 2:
		projection = None
	ax = fig.add_subplot(2, 2, idx, projection=projection)
	ax.set_title(title)
	cmap = plt.cm.get_cmap('hsv', k + 1)
	for (idx, point) in enumerate(points):
		ax.scatter(*point, color=cmap(clusters[idx]))
	return fig

def write_mapping_result(f, infered_k, mapping):
	lines = [[] for _ in range(infered_k)]
	for (idx, mapped) in enumerate(mapping):
		lines[mapped].append(idx)
	for line in lines:
		f.write(','.join([str(val) for val in line]) + '\n')

def handle_results(points, spectral_mapping, kmeans_mapping, orig_mapping, orig_n, orig_k, dims, infered_k):
	pp = PdfPages('clusters.pdf')
	fig = plt.figure(figsize=(8,6))
	fig = visualize_results(points, spectral_mapping, infered_k, dims, fig, 1, "Normalized Spectral Clustering")
	fig = visualize_results(points, kmeans_mapping, infered_k, dims, fig, 2, "K-means")
	text = "Data was generated from the values:\n\
n = {n}, k = {orig_k}\n\
The k that was used for both algorithms was {infered_k}\n\
The Jaccard measure for Spectral Clustering: {spectral_jac}\n\
The Jaccard measure for K-means: {kmeans_jac}"\
	.format(n=orig_n, orig_k=orig_k, infered_k=infered_k, 
		spectral_jac=calc_jac(orig_mapping, spectral_mapping), 
		kmeans_jac=calc_jac(orig_mapping, kmeans_mapping))

	fig.text(.5, .05, text, ha='center')
	pp.savefig(fig)
	pp.close()

	with open('clusters.txt', 'w') as f:
		f.write(str(infered_k) + '\n')
		write_mapping_result(f, infered_k, spectral_mapping)
		write_mapping_result(f, infered_k, kmeans_mapping)





def main():
	args = get_args()
	points, mapping, dims = generate_blobs(args.n, args.k, args.random)
	spectral_mapping, infered_k = spectral_pp.spectral_main(args.n, dims, points.tolist())
	# We'll be using the K yielded from spectral rather than the argument given for both spectral and kmeans.
	kmeans_mapping = kmeans_pp.kmeans_main(infered_k, points.tolist())
	handle_results(points, spectral_mapping, kmeans_mapping, mapping, args.n, args.k, dims, infered_k)

if __name__ == "__main__":
	main()
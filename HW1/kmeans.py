import argparse
import math

class Cluster(object):
	# Cluster class for holding points as centroid
	def __init__(self, center, dims): 
		self.dims = dims
		self.center = center
		self.points = {center}
		self.axis_sums = center

	def distance(self, point):
		# Calculate distance from point to center
		return math.sqrt(sum([(p1 - p2) ** 2 for (p1 , p2) in zip(self.center, point)]))

	def add_point(self, point):
		# Add point and recalculate center
		self.points.add(point)
		self.axis_sums = [p1 + p2 for (p1, p2) in zip(self.axis_sums, point)]
		self.center = tuple([axis / len(self.points) for axis in self.axis_sums])

	def remove_point(self, point):
		# Remove point and recalculate center
		self.points.remove(point)
		self.axis_sums = [p1 - p2 for (p1, p2) in zip(self.axis_sums, point)]
		self.center = tuple([axis / len(self.points) for axis in self.axis_sums])

class NoneCluster(object):
	# NoneCluster class for first iteration
	def __init__(self):
		self.center = None

	def distance(self, point):
		return math.inf
	
	def remove_point(self, point):
		pass

def get_args():
	# Fetch arguments from commandline
	parser = argparse.ArgumentParser()
	parser.add_argument("K", help="the number of clusters required", type=int)
	parser.add_argument("N", help="the number of observations in the file", type=int)
	parser.add_argument("d", help="the dimension of each observation and initial centroids", type=int)
	parser.add_argument("MAX_ITER", help="the maximum number of iterations of the K-means algorithm", type=int)
	args = parser.parse_args()
	assert args.K < args.N, "K must be smaller than N"
	return args

def get_input(dim):
	# Fetch the raw data inputted and parse as points
	# Verify every point is of the correct dimension
	points = []
	while True:
		try:
			point = tuple((float(val) for val in input().split(",")))
			assert len(point) == dim, "Input data has a point of wrong dimension"
			points.append(point)
		except EOFError:
			break
	return points

def parse_input():
	# Fetch all the program input
	args = get_args()
	points = get_input(args.d)
	return args.K, args.N, args.d, args.MAX_ITER, points

def init_centroids(points, K, d):
	# Initialize all centroids, creating NoneCentroids for the points without a centroid
	centroids = tuple(Cluster(point, d) for point in points[:K])
	points_to_centroids = {point:centroid for (point, centroid) in zip(points, centroids)}
	NONE_CENTROID = NoneCluster()
	for point in points[K:]:
		points_to_centroids[point] = NONE_CENTROID
	return centroids, points_to_centroids

def kmeans(centroids, points_to_centroids):
	change_dict = {}
	# Createing a dict containing a mapping from a point, to new centroid if changed
	for point in points_to_centroids.keys():
		min_dst = math.inf
		designated_centroid = None
		for centroid in centroids:
			# Calculate what centroid is the closest to a point
			dst = centroid.distance(point)
			if dst < min_dst:
				min_dst = dst
				designated_centroid = centroid
		if points_to_centroids[point] != designated_centroid:
			change_dict[point] = designated_centroid

	for point in change_dict.keys():
		# Execute the saved changes once all changes have been established
		points_to_centroids[point].remove_point(point)
		points_to_centroids[point] = change_dict[point]
		points_to_centroids[point].add_point(point)

	return centroids, change_dict == {}

def run_kmeans(centroids, points, MAX_ITER):
	# Run kmeans for MAX_ITER iterations
	for i in range(MAX_ITER):
		centroids, changed = kmeans(centroids, points)
		if not changed:
			break
	return centroids

def main():
	K, N, d, MAX_ITER, points = parse_input()
	centroids, points_to_centroids = init_centroids(points, K, d)
	centroids = run_kmeans(centroids, points_to_centroids, MAX_ITER)
	for centroid in centroids:
            print(",".join(["{:.2f}".format(axis) for axis in centroid.center]))



if __name__ == "__main__":
	main()

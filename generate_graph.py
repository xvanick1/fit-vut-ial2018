#!/usr/bin/env python3

'''
Script to generate undirectional graph formated as 
matrix of connections. 
Created graph can be:
1. Randomly connected
2. Fully connected 
3. Empty of connections

Use:
python3 generate_graph.py -MODE -o OUTPUT_FILE -n NUMBER_OF_NODES

TODO: Measure time of certain graphs and compare with algorithm complexity
'''

import argparse
import random
import numpy as np

def arguments():

	parser = argparse.ArgumentParser()

	mode = parser.add_mutually_exclusive_group(required=True)
	mode.add_argument("-r", action='store_true',
		help="create graph with random connections")
	mode.add_argument("-f", action='store_true',
		help="create fully connected graph")
	mode.add_argument("-e", action='store_true',
		help="create empty graph (no connections)")

	required = parser.add_argument_group('required arguments')
	required.add_argument("-o", "--output",
		help="file in which graph matrix will be written", required=True)
	required.add_argument("-n", "--nodes", 
		help="number of nodes in graph", required=True)

	return parser.parse_args()

def create_matrix(args, n):

	# Randomly connected graph
	if args.r is True:

		# Create zeroed numpy 2D array
		matrix = np.full((n, n), 0)

		# Randomize connections only at and above main diagonal of matrix
		m = 0
		for i in range(0, n):
			for j in range(m, n):
				matrix[i][j] = random.randint(0, 1)
			m += 1

		'''
		Make matrix symmetrical by main diagonal by 
		transposing it and making logical OR operation on it
		'''
		matrix = np.logical_or(matrix, matrix.T).astype(int)

	# Fully connected graph
	elif args.f is True:
		matrix = np.full((n, n), 0)

	# Empty of connections
	elif args.e is True:
		matrix = np.full((n, n), 0)

	return matrix

def main():

	# Parsing arguments
	args = arguments()
	outputFile = open(args.output, 'w+')
	n = int(args.nodes)
	outputFile.write(str(n) + "\n")

	# Fill matrix based on mode
	matrix = create_matrix(args, n)

	# Print graph matrix to output file
	for i in range(0, n):
		for j in range(0, n):
			outputFile.write(str(matrix[i][j]) + " ")
		outputFile.write("\n")

	outputFile.close()

if __name__ == "__main__":
	main()
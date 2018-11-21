#!/usr/bin/env python3

'''
Script to generate undirectional graph formated as 
matrix of connections. 
Created graph can be:
1. Randomly connected
2. Fully connected (without self-loops)
3. Empty of connections

Use:
python3 generate_graph.py -MODE -o OUTPUT_FILE -n NUMBER_OF_NODES

'''

import argparse
import random
import numpy as np

def arguments():

	parser = argparse.ArgumentParser()

	mode = parser.add_mutually_exclusive_group()
	mode.add_argument("-f", "--full", action='store_true',
		help="create fully connected graph (default)")
	mode.add_argument("-r", "--random", action='store_true',
		help="create graph with random connections")
	mode.add_argument("-e", "--empty", action='store_true',
		help="create empty graph (no connections)")
	
	parser.add_argument("-o", "--output",
		help="file in which graph matrix will be written (defaults to \"nodes.txt\")")

	required = parser.add_argument_group('required arguments')
	required.add_argument("-n", "--nodes", 
		help="number of nodes in graph", required=True)

	return parser.parse_args()

def create_matrix(args, n, default_full):

	# Randomly connected graph
	if args.random:

		# Create zeroed numpy 2D array
		matrix = np.full((n, n), 0)

		# Randomize connections only at and above main diagonal of matrix
		m = 1
		for i in range(0, n):
			for j in range(m, n):
				matrix[i][j] = random.randint(0, 1)
			m += 1

		'''
		Make matrix symmetrical by main diagonal by 
		transposing it and adding it to that
		'''
		matrix = matrix + matrix.T

	# Fully connected graph (without self-loops)
	elif args.full or default_full:
		matrix = np.full((n, n), 1)
		np.fill_diagonal(matrix, 0)

	# Empty of connections
	elif args.empty:
		matrix = np.full((n, n), 0)

	return matrix

def main():

	# Parsing arguments
	args = arguments()

	# If filename is not provided, script will write to nodes.txt
	if args.output:
		outputFile = open(args.output, 'w+')
	else:
		outputFile = open("nodes.txt", 'w+')

	# If mode is not provided, default to fully connected
	default_full = False
	if not (args.random or args.full or args.empty):
		default_full = True

	# Write number of nodes to file
	n = int(args.nodes)
	outputFile.write(str(n) + "\n")

	# Fill matrix based on mode
	matrix = create_matrix(args, n, default_full)

	# Print graph matrix to output file
	for i in range(0, n):
		for j in range(0, n):
			outputFile.write(str(matrix[i][j]) + " ")
		outputFile.write("\n")

	outputFile.close()

if __name__ == "__main__":
	main()
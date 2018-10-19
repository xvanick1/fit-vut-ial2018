#!/usr/bin/env python3

'''
Testing script of graph coloring program

Use:
$ python3 testing.py

Tests must be in "tests/" folder with format:
- "testX.in" for input graph
- "testX.out" for output (minimal chromatic number)

TODO: Run tests only from X to Y for convenience
'''

import argparse
import subprocess # For calling C program from Python

class bcolors:
    OKGREEN = '\033[92m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'

def arguments():

	parser = argparse.ArgumentParser()

	parser.add_argument('-m',
		help="max tests to run (default is all tests)")

	return parser.parse_args()

def execute_tests(num_of_tests):

	passed = 0
	not_passed = []

	''' Compare every program output with expected value '''
	for x in range(1, num_of_tests + 1):

		test_in = "tests/test{}.in".format(x)
		test_out = "tests/test{}.out".format(x)

		''' Feed C program with test graph and get its output ''' 
		out = subprocess.Popen(["./main", test_in], 
			stdout=subprocess.PIPE).stdout.readline()
		out = int(out)

		''' Read test file '''
		test_out_file = open(test_out, 'r')
		expected = int(test_out_file.read())

		''' Compare program output with test output '''
		if out == expected:
			passed += 1
			print("TEST {}: output [{}] == expected [{}]".
				format(x, out, expected) 
				+ bcolors.OKGREEN + bcolors.BOLD + " => PASSED" 
				+ bcolors.ENDC)
		else:
			not_passed.append(x)
			print("TEST {}: output [{}] != expected [{}]".
				format(x, out, expected) 
				+ bcolors.FAIL + bcolors.BOLD + " => WRONG" 
				+ bcolors.ENDC)

	return passed, not_passed

def main():

	''' Parsing arguments '''
	args = arguments()

	''' Get number of tests '''
	test_files = 0
	for line in subprocess.Popen(["ls", "tests", '-1'], # This is -one actually
			stdout=subprocess.PIPE).stdout:
		test_files += 1
	num_of_tests = test_files // 2

	''' If specified, only given number of tests will be run '''
	if args.m:
		max_tests = int(args.m)
		if max_tests and max_tests <= num_of_tests:
			num_of_tests = max_tests

	''' Iterate through all tests ''' 
	passed, not_passed = execute_tests(num_of_tests)

	''' Print final outcome '''
	if passed == num_of_tests:
		print("\n" + bcolors.OKGREEN + bcolors.BOLD 
			+ "Passed {} out of {} tests".format(passed, num_of_tests)
			+ bcolors.ENDC + "\n")
	else:
		print("\n" + bcolors.FAIL + bcolors.BOLD 
			+ "Passed {} out of {} tests".format(passed, num_of_tests)
			+ bcolors.ENDC)
		print("Tests not passed: ", end = "")
		for x in not_passed:
			print("{}, ".format(x), end = "")
		print("\n")

if __name__ == "__main__":
	main()
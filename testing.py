#!/usr/bin/env python3

'''
Testing script of graph coloring program

Use:
$ python3 testing.py [-a A] [-b B]
	A ... first test
	B ... last test

Tests must be in "tests/" folder with format:
- "testX.in" for input graph
- "testX.out" for output (minimal chromatic number)

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

	interval_args = parser.add_argument_group('arguments specifying testing interval')
	interval_args.add_argument('-a',
		help="beginning of interval (defaults to first test)")
	interval_args.add_argument('-b',
		help="end of interval (defaults to last test)")

	return parser.parse_args()

def execute_tests(a, b):

	passed = 0
	not_passed = []

	''' Run every test in given interval '''
	for x in range(a, b + 1):

		''' Get test filenames '''
		test_in = "tests/test{}.in".format(x)
		test_out = "tests/test{}.out".format(x)

		''' Feed C program with test graph and get its output ''' 
		out = subprocess.Popen(["./main", test_in, "-b"], 
			stdout=subprocess.PIPE).stdout.readline()
		out = int(out)

		''' Read test output file '''
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

	''' Get number of tests from "tests" folder '''
	test_files = 0
	for line in subprocess.Popen(["ls", "tests", '-1'], # This is -one actually
			stdout=subprocess.PIPE).stdout:
		test_files += 1
	num_of_tests = test_files // 2

	''' Set default interval of tests (all) '''
	a = 1
	b = num_of_tests

	''' If specified, only given interval of tests will be run '''
	if args.a:
		if 0 < int(args.a) <= num_of_tests:
			a = int(args.a)
	if args.b:
		if 0 < int(args.b) <= num_of_tests:
			b = int(args.b)

	''' Iterate through tests in interval < a ; b > ''' 
	passed, not_passed = execute_tests(a, b)

	''' Print final outcome '''
	interval = (b - a) + 1 # number of executed tests
	if passed == interval:
		print("\n" + bcolors.OKGREEN + bcolors.BOLD 
			+ "Passed {} out of {} tests ({} total)".
			format(passed, interval, num_of_tests)
			+ bcolors.ENDC + "\n")
	else:
		print("\n" + bcolors.FAIL + bcolors.BOLD 
			+ "Passed {} out of {} tests ({} total)".
			format(passed, interval, num_of_tests)
			+ bcolors.ENDC)
		print("Tests not passed: ", end = "")
		for x in not_passed:
			print("{}, ".format(x), end = "")
		print("\n")

if __name__ == "__main__":
	main()
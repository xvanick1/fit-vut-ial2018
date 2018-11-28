# matrix = [[0, 1, 1, 0, 0, 0, 0, 0, 0, 0],
# [1, 0, 0, 1, 0, 0, 0, 0, 0, 0],
# [1, 0, 0, 1, 1, 0, 0, 1, 0, 0],
# [0, 1, 1, 0, 0, 1, 1, 0, 0, 0],
# [0, 0, 1, 0, 0, 0, 1, 1, 0, 0],
# [0, 0, 0, 1, 0, 0, 1, 1, 0, 0],
# [0, 0, 0, 1, 1, 1, 0, 1, 1, 0],
# [0, 0, 1, 0, 1, 1, 1, 0, 0, 1],
# [0, 0, 0, 0, 0, 0, 1, 0, 0, 1],
# [0, 0, 0, 0, 0, 0, 0, 1, 1, 0]]
# num_of_nodes = 10

matrix = [[0, 1, 1, 0, 1, 0],
 [1, 0, 0, 1, 0, 1],
 [1, 0, 0, 0, 1, 1],
 [0, 1, 0, 0, 1, 1],
 [1, 0, 1, 1, 0, 1],
 [0, 1, 1, 1, 1, 0]]
num_of_nodes = 6

# matrix = [[0, 1, 1, 0],
#  [1, 0, 1, 0],
#  [1, 1, 0, 1],
#  [0, 0, 1, 0]]
# num_of_nodes = 4

node_array = []

class Node:
	nid = -1
	color = -1
	colorset = None
	num_of_remaining_values = -1 # for most constrained heuristics
	num_of_neighbors = -1 # for most constraining heuristics


def print_sets(i):
	print("Node {} color: {}".format(i,node_array[i].color))
	for node in node_array:
		print("Node {} = {{ ".format(node.nid), end='')
		for color in node.colorset:
			print(color, end=' ')
		print("}")

def print_colors():
	print()
	for x in range(0,num_of_nodes):
		color = node_array[x].color
		if color == -1:
			print('-',' ', end='')
		else:
			print(node_array[x].color, ' ', end='')
	print()

def forward_checking(i):
	# print("\nLEVEL", i)

	''' So it seems that on every level of recursion I need to keep
	records of colorsets of nodes from i+1 to n, so I can roll back 
	in case of need '''
	node_sets = [None for x in node_array]
	for node in node_array[i+1:]:
		node_sets[node.nid] = node.colorset.copy()

	while True:

		''' Here I will maybe have to assign to node_sets 
		new values, so it works in loop, but not sure... '''

		''' Delete first color from colorset and assign it to node '''
		node_array[i].color = node_array[i].colorset.pop(0)

		''' Choosing most constrained variable '''
		# chosen_node = None
		# min_num_of_legal_values = num_of_nodes - 1
		# for node in node_array:
		# 	# First must choose from nodes without assigned color
		# 	if node.color == -1:
		# 		if len(node.colorset) < min_num_of_legal_values:
		# 			min_num_of_legal_values = len(node.colorset)
		# 			chosen_node = node.nid
		# # Now we need to delete first color from colorset and assign it
		# node_array[chosen_node].color = node_array[chosen_node].colorset.pop()

		# print_colors();
		# print_sets(i)

		''' Goal is reached '''
		if i == (num_of_nodes - 1):
			return True

		''' For checking if some colorset is empty '''
		goback_flag = False

		''' Iterate through all nodes with index i + 1 '''
		for node in node_array[i+1:]:

			''' Iterate through all nodes with assigned color '''
			for node_assigned in node_array[:i+1]: 

				''' Neighbor was found '''
				if matrix[node.nid][node_assigned.nid] == 1:

					''' In case of conflict, delete color from colorset '''
					for color in node.colorset:
						if color == node_assigned.color:
							# print("Conflict between node", node_assigned.nid,
							# 	"and node", node.nid, ": color", color)
							''' Erase color on the right index '''
							index = node.colorset.index(color)
							node.colorset.pop(index)
							# print("Node", node.nid, "saved colorset:", node_sets[node.nid])
							# print("Node", node.nid, "colorset:", node.colorset)
				
			''' If some colorset is empty, this combination won't work,
			and so it's needed to take next color from i-th node,
			or declare failure and try different value with i-1-th node '''
			if not node.colorset:
				# print("Colorset of node", node.nid, "is empty")
				goback_flag = True

		''' If some colorset of nodes from i+1 to n is empty, give them 
		back their colors from before, so another color of i-th node 
		can be tried '''
		if goback_flag:
			for node in node_array[i+1:]:
				node.colorset = node_sets[node.nid]
		else:
			success = forward_checking(i+1)
			if success:
				return True
			else:
				''' Here I guess I need to rewrite colorsets of nodes, so 
				they no longer have colorsets from that recursive call '''
				for node in node_array[i+1:]:
					node.colorset = node_sets[node.nid]


		''' If colorset of i-th node is empty, declare fail, else continue
		looping through other colors in the colorset '''
		if not node_array[i].colorset:
			# print("Colorset of node", i, "is empty, going up")
			node_array[i].color = -1
			return False
		else:
			continue

def main():

	# Initialize nodes and node array
	for x in range(0,num_of_nodes):
		node = Node()
		node.nid = x
		# node.colorset = [0,1,2,3]
		node_array.append(node)

	# Every round colorsets of nodes will get more colors
	for fc_round in range(0,num_of_nodes):

		for node in node_array:
			node.colorset = []
			for color in range(0,fc_round + 1):
				node.colorset.append(color)

			# Need to initialize colors
			node.color = -1

		if forward_checking(0):
			print("RESULT: SUCCESS")
			print_colors();
			break;
		else:
			print("RESULT: FAILURE")
			print_colors();

if __name__ == "__main__":
	main()
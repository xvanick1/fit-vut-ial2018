matrix = [[0, 1, 1, 0, 0, 0, 0, 0, 0, 0],
[1, 0, 0, 1, 0, 0, 0, 0, 0, 0],
[1, 0, 0, 1, 1, 0, 0, 1, 0, 0],
[0, 1, 1, 0, 0, 1, 1, 0, 0, 0],
[0, 0, 1, 0, 0, 0, 1, 1, 0, 0],
[0, 0, 0, 1, 0, 0, 1, 1, 0, 0],
[0, 0, 0, 1, 1, 1, 0, 1, 1, 0],
[0, 0, 1, 0, 1, 1, 1, 0, 0, 1],
[0, 0, 0, 0, 0, 0, 1, 0, 0, 1],
[0, 0, 0, 0, 0, 0, 0, 1, 1, 0]]
num_of_nodes = 10

# matrix = [[0, 1, 1, 0, 1, 0],
#  [1, 0, 0, 1, 0, 1],
#  [1, 0, 0, 0, 1, 1],
#  [0, 1, 0, 0, 1, 1],
#  [1, 0, 1, 1, 0, 1],
#  [0, 1, 1, 1, 1, 0]]
# num_of_nodes = 6

# matrix = [[0, 1, 1, 0],
#  [1, 0, 1, 0],
#  [1, 1, 0, 1],
#  [0, 0, 1, 0]]
# num_of_nodes = 4

class Node:
	nid = -1
	color = -1
	colorset = None # len(colorset) for most constrained var heuristics
	neighbors = None # for most constraining var heuristics


# def print_sets(i):
# 	print("Node {} color: {}".format(i,node_array[i].color))
# 	for node in node_array:
# 		print("Node {} = {{ ".format(node.nid), end='')
# 		for color in node.colorset:
# 			print(color, end=' ')
# 		print("}")

def print_colors(assigned_nodes, unassigned_nodes):
	# print()
	# print('Nodes : ',end='')
	# for x in range(0,len(assigned_nodes)):	
	# 	for node in assigned_nodes:
	# 		if node.nid == x:
	# 			print(node.nid, ' ', end='')
	print('\nColors: ',end='')
	for x in range(0,len(assigned_nodes)):	
		for node in assigned_nodes:
			if node.nid == x:
				color = node.color
				if color == -1:
					print('-', ' ', end='')
				else:
					print(node.color, ' ', end='')
	print()

''' Gets degree of UNASSIGNED neighbors'''
def get_degree(node):
	degree = 0 
	for neighbor in node.neighbors:
		if neighbor.color == -1:
			degree += 1
	return degree

''' Choosing next node using heuristics for forward checking 
1. Most constrained variable
2. Most constraining variable '''
def choose_node(unassigned_nodes):
	''' Unfortunately that degree heuristics won't be so easy,
	since it's about degree of UNASSIGNED neighbors, which the node can
	constrain (it's not constraining the assigned ones...) '''

	chosen_node = unassigned_nodes[0]
	last_degree = get_degree(chosen_node)
	for node in unassigned_nodes[1:]:
		if len(node.colorset) < len(chosen_node.colorset):
			# Get degree of unassigned neighbors
			last_degree = get_degree(node)
			chosen_node = node
		# In case of tie, save node only if it has bigger degree
		# of unassigned neighbors
		elif len(node.colorset) == len(chosen_node.colorset):
			# Get degree of unassigned neighbors
			degree = get_degree(node)
			if degree > last_degree:
				last_degree = degree
				chosen_node = node
	return chosen_node

def forward_checking(i, assigned_nodes, unassigned_nodes):
	print("\nLEVEL", i)

	''' So it seems that on every level of recursion I need to keep
	records of colorsets of nodes from i+1 to n, so I can roll back 
	in case of need '''
	node_sets = [None for x in range(0,num_of_nodes)]
	for node in unassigned_nodes:
		node_sets[node.nid] = node.colorset.copy()

	''' Choosing most constrained variable '''
	chosen_node = choose_node(unassigned_nodes)
	# Chosen node is now last node in assigned array
	assigned_nodes.append(chosen_node)
	unassigned_nodes.remove(chosen_node)

	while True:
		# HERE is place for color choosing heuristics


		# Now we need to delete FIRST color from colorset and assign it
		assigned_nodes[-1].color = assigned_nodes[-1].colorset.pop(0)

		print_colors(assigned_nodes, unassigned_nodes);
		# print_sets(i)

		''' Goal is reached, when unassigned array is empty '''
		if not unassigned_nodes:
			return True
		# if i == (num_of_nodes - 1):
		# 	return True

		''' For checking if some colorset is empty '''
		goback_flag = False

		''' Iterate through all unassigned nodes '''
		for unassigned in unassigned_nodes:

			''' Iterate through all nodes with assigned color '''
			for assigned in assigned_nodes:

				''' Neighbor was found '''
				if matrix[unassigned.nid][assigned.nid] == 1:

					''' In case of conflict, delete color from colorset '''
					for color in unassigned.colorset:
						if color == assigned.color:
							# print("Conflict between node", node_assigned.nid,
							# 	"and node", node.nid, ": color", color)
							''' Erase color on the right index '''
							index = unassigned.colorset.index(color)
							unassigned.colorset.pop(index)
							# print("Node", node.nid, "saved colorset:", node_sets[node.nid])
							# print("Node", node.nid, "colorset:", node.colorset)
				
			''' If some colorset is empty, this combination won't work,
			and so it's needed to take next color from i-th node,
			or declare failure and try different value with i-1-th node '''
			if not unassigned.colorset:
				# print("Colorset of node", node.nid, "is empty")
				goback_flag = True

		''' If some colorset of unassigned nodes is empty, give them back
		their colors from before, so another color of node, that has
		its turn, can be tried '''
		if goback_flag:
			for node in unassigned_nodes:
				node.colorset = node_sets[node.nid]
		else:
			success = forward_checking(i+1, assigned_nodes, unassigned_nodes)
			if success:
				return True
			else:
				''' Here I guess I need to rewrite colorsets of nodes, so 
				they no longer have colorsets from that recursive call '''
				for node in unassigned_nodes:
					node.colorset = node_sets[node.nid]

		''' If colorset of node that has its turn is empty, declare fail, 
		else continue looping through other colors in the colorset '''
		# choose last node in assigned array
		if not assigned_nodes[-1].colorset:
			# print("Colorset of node", i, "is empty, going up")
			assigned_nodes[-1].color = -1
			# pop last node from assigned array and append it to end of 
			# unassigned array
			unassigned_nodes.append(assigned_nodes.pop())
			return False
		else:
			continue

def main():

	# Every round colorsets of nodes will get more colors
	for fc_round in range(0,num_of_nodes):

		# Initialize nodes and node arrays
		assigned_nodes = []
		unassigned_nodes = []
		for x in range(0,num_of_nodes):
			node = Node()
			node.nid = x
			node.color = -1
			# Finally add node to array
			unassigned_nodes.append(node)

		# Also need to get list of neighbors for 2nd heuristic
		for node in range(0,num_of_nodes):
			unassigned_nodes[node].neighbors = []
			for neighbor in range(0,num_of_nodes):
				if matrix[node][neighbor] == 1:
					unassigned_nodes[node].neighbors.append(unassigned_nodes[neighbor])

		for node in unassigned_nodes:
			# Every round colorsets will get one more color
			node.colorset = []
			for color in range(0,fc_round + 1):
				node.colorset.append(color)

		if forward_checking(0, assigned_nodes, unassigned_nodes):
			print("RESULT: SUCCESS")
			print_colors(assigned_nodes, unassigned_nodes);
			break;
		else:
			print("RESULT: FAILURE")
			print_colors(assigned_nodes, unassigned_nodes);

if __name__ == "__main__":
	main()
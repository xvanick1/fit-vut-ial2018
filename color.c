#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "stack.h" // For Node and NodeStack structs

/* Processes all neighbors and returns available color */
int get_color(Node* node, Node *node_array,	int num_of_nodes, 
	int min_chromatic_num) {

	/* For saving colors of neighbors and then finding lowest 
	available */ 
	bool *colors = calloc(num_of_nodes + 1, sizeof(*colors));

	/* Initialize all colors as available */
	for(int i = 1; i < num_of_nodes + 1; i++) {
		colors[i] = true;
	}

	/* Mark colors of neighbors as unavailable */
	int num_of_neighbors = node->num_of_neighbors;
	for(int i = 0; i < num_of_neighbors; i++) {

		/* Iterating through all neighbors by index */
		int neighbor_id = node->neighbors[i];

		/* Set color of neighbor as unavailable, even when 
		color is 0 (none) */
		colors[node_array[neighbor_id].color] = false;
	}

	/* Choosing the lowest available color */
	int available_color = 0;
	/* Minimal color must be node's last color + 1, so that the node
	iterate through all its possible colors.
	Also it shouldn't get color bigger than saved minimal chromatic
	number, so it doesn't unnecessarily iterate through higher colors */
	for(int i = node->color + 1; i <= min_chromatic_num; i++) {
		if(colors[i] == true) {
			available_color = i;
			break;
		}
	}
	free(colors);

	/* available_color == 0 means this node won't get any color */
	return available_color;
}

/* Makes sure only minimal solution gets saved to array */
void success(Node* node_array, int num_of_nodes, int *min_colored_array,
	int *min_chromatic_num) {

	/* Getting max color from node array*/
	int max_color = 0;
	for (int i = 0; i < num_of_nodes; i++) {
		if(node_array[i].color > max_color) {
			max_color = node_array[i].color;
		}
	}
	
	/* Making sure I save color array only with 
	smaller chromatic number */
	if(max_color < *min_chromatic_num) {

		*min_chromatic_num = max_color;
		for(int i = 0; i < num_of_nodes; i++) {
			min_colored_array[i] = node_array[i].color;
		}
		printf("\nSMALLER ");
	}

	printf("SOLUTION: ");
	for (int i = 0; i < num_of_nodes; ++i) {
		printf(" %d", node_array[i].color);
	}
	printf("\n");
}

void backtracking_csp(Node *node_array, NodeStack *stack, int num_of_nodes) {

	/* min_chromatic_num must be bigger by 1 than num_of_nodes, so
	graphs with property colors == nodes get solution saved to array */
	int min_chromatic_num = num_of_nodes + 1;
	int *min_colored_array = calloc(num_of_nodes, sizeof(*min_colored_array));

	/* Pushing first node to stack */
	int id = 0;
	stack_push(stack, &(node_array[id]));

	while(42) {

		/* No more nodes on stack means all wanted color variations 
		were tried already */
		if(stack_empty(stack)) {
			break;
		}
		else {

			/* Choose color for node on top of stack */ 
			Node *node = stack_top(stack);
			node->color =
				get_color(node, node_array, num_of_nodes, min_chromatic_num);
			
			/* Print depth levels */
			// for (int i = 0; i < id; ++i) printf("\t");
			// printf("Node %d - %d\n", node->id, node->color);

			/* Node can be colored (rule can be applied) */
			if(node->color != 0) {

				/* One of goals was reached (last node in array
				was colored) */
				if(node->id == num_of_nodes - 1) { 
					success(node_array, num_of_nodes, min_colored_array, 
						&min_chromatic_num);
				}
				/* Go down the tree */
				else {
					id++;
					stack_push(stack, &(node_array[id]));
					continue;
				}
			}
			/* Go up the tree */
			else {
				/* Making sure color of node will be 0 for next encounters */
				node->color = 0; // not sure if needed

				id--;
				stack_pop(stack);
				continue;
			}
		}
	}

	/* Print minimal solution */
	printf("\nMinimal chromatic number: %d\n", min_chromatic_num);
	printf("MINIMAL SOLUTION: ");
	for(int i = 0; i < num_of_nodes; i++) {
		printf(" %d", min_colored_array[i]);
	}
	printf("\n");

	free(min_colored_array);
}

void fill_node(FILE* file, Node *node, int num_of_nodes) {

	/* Getting node id and initializing it */
	char b;
	fscanf(file, " %d:%c", &(node->id), &b);
	node->color = 0;
	node->num_of_neighbors = 0;
	node->neighbors = NULL;

	/* Node has no neighbors */
	if(b == ';') {
		printf("\n");
		return;
	}
	else {
		/* Getting node neighbors and their count */
		int temp_array[num_of_nodes];
		char c; // For recognizing semicolon as end of line
		int n = 0; // Number of neighbors
	    do {
			fscanf(file, "%d%c", &(temp_array[n]), &c);
	    	n++;
	    } while(c != ';');
	    

	    /* Allocating memory for array of neighbors 
	    and copying to it from temporary array */
	    node->num_of_neighbors = n;
	    node->neighbors = calloc(n, sizeof(*node->neighbors));
	    for(int j = 0; j < n; j++) {
	    	node->neighbors[j] = temp_array[j];
	    }
	}
}

void print_nodes(Node *node_array, int num_of_nodes) {
	printf("Num of nodes: %d\n", num_of_nodes);
	for (int i = 0; i < num_of_nodes; i++)
	{
		printf("Node id: %d ", node_array[i].id);
		printf("(");
		for (int j = 0; j < node_array[i].num_of_neighbors; j++)
		{
			printf(" %d", node_array[i].neighbors[j]);
		}
		printf(" )\n");
	}
}

Node *scan_file(char* filename) {

	FILE *file;
    file = fopen(filename,"r");
    if(file == NULL) {
        fprintf(stderr, "ERROR: File opening failed\n");
        exit(-1);
    }

    /* Getting number of nodes */
    fscanf(file, "count = %d;", &num_of_nodes);

    /* Node array where nodes are expected to be organized by id 
    for simple lookup by index of array */
    Node *node_array = calloc(num_of_nodes, sizeof(*node_array));

    /* Getting nodes from file */
    for(int i = 0; i < num_of_nodes; i++) {
    	fill_node(file, &node_array[i], num_of_nodes);
    }

    fclose(file); 

    print_nodes(node_array, num_of_nodes);

    return node_array;
}

int main(int argc, char* argv[]) {

	// TODO: Error handling
	// TODO: Change format of nodes in file

	if(argc != 2) {
		fprintf(stderr, "ERROR: Not enough arguments. Run with './main nodes.txt'\n");
		exit(-1);
	}

	/* Get graph info from file */
	Node *node_array = scan_file(argv[1]);

    /* Seting up node stack */
    NodeStack stack;
	stack_init(&stack);
	stack.array = calloc(num_of_nodes, sizeof(*stack.array));

    /* Do something with nodes */
    backtracking_csp(node_array, &stack, num_of_nodes);

    /* Freeing memory */
    for(int i = 0; i < num_of_nodes; i++) {
    	if(node_array[i].num_of_neighbors > 0) {
	    	free(node_array[i].neighbors);
    	}
    }
    free(node_array);
    free(stack.array);

	return 0;
}
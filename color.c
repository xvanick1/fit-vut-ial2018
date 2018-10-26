/* 

FILE NAME: color.c

PROJECT IDENTIFICATION: IAL, "6. Obarvení grafu"

AUTHORS:
- Josef Adamek (xadame42)
- Diana Barnova (xbarno00)
- Jozef Vanicky (xvanic09)
- Filip Weigel (xweige01)

BRIEF DESCRIPTION:
Main part of coloring program

DATE OF CREATION:
27.9.2018

DATE OF LAST CHANGE:
---

_________________________________________________

USE:
$ ./main nodes.txt [-p]
- "-p" flag turns on printing of info while program is running

NOTES:
- in this implementation self-loops (node connected to itself) 
and multiple edges between nodes are NOT allowed

TODO:
- print MANUAL when ran with wrong arguments
- headers in each program file
- error handling
- in case of error, free all allocated memory
- maybe try forward-checking
- program needs to return solution, not just min. chrom. number

*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#include "stack.h" // For global variables and structures

/* For printing solution */
typedef enum {
	NORMAL,
	SMALLER,
	MINIMAL
} MODE;

/* Prints coloring of nodes */
void print_coloring(int *min_colored_array, int min_chromatic_num, 
	int mode) {

	if(!print_flag) return;

	if(mode == NORMAL || mode == SMALLER) {

		if(mode == NORMAL) 
			printf("CHROM. NUMBER {%d}: ", min_chromatic_num);
		else 
			printf("SMALLER CHROM. NUMBER {%d}: ", min_chromatic_num);

		for (int i = 0; i < num_of_nodes; ++i) {
			printf(" %d", node_array[i].color);
		}
		printf("\n");
	}

	else if(mode == MINIMAL) {
		printf("\nMINIMAL CHROM. NUMBER {%d}: ", min_chromatic_num);
		for(int i = 0; i < num_of_nodes; i++) {
			printf(" %d", min_colored_array[i]);
		}
		printf("\n");
	}

	else {
		fprintf(stderr, "ERROR!\n");
	}
}

/* Prints nodes info */
void print_nodes(int num_of_nodes) {

	if(!print_flag) return;

	printf("Num of nodes: %d\n", num_of_nodes);
	for (int i = 0; i < num_of_nodes; i++)
	{
		printf("Node id: %d ", node_array[i].id);
		printf("(");
		for (int j = 0; j < num_of_nodes; j++)
		{
			if(graph_table[i][j] == true) {
				printf(" %d", j);
			}
		}
		printf(" )\n");
	}
	printf("\n");
}

/* Checks if matrix representing graph is symmetrical by diagonal
and that it doesn't contain self-loops. Otherwise it ends program */
void check_matrix() {
	int diagonal = 0;
	for (int i = 0; i < num_of_nodes; i++)
	{
		if(graph_table[i][diagonal] == 1) {
			fprintf(stderr, "Self-loops are not allowed!\n");
			exit(1);
		}
		for (int j = diagonal; j < num_of_nodes; j++)
			if(graph_table[i][j] != graph_table[j][i]) 
			{
				fprintf(stderr, "Input graph is not undirected!\n");
				exit(1);
			}
		diagonal++;
	}
}

/* Makes sure only minimal solution gets saved to array */
void success(NodeStack *stack, int *min_colored_array, int num_of_nodes, 
	int *min_chromatic_num) {

	/* Getting max color from node array */
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

		/* Print better solution */
		print_coloring(min_colored_array, *min_chromatic_num, SMALLER);

		/* OPTIMIZATION #2
		Find the first node from left in min_colored_array, that has color
		same as min_chromatic_num and pop it and all neighbors next to its
		right, because this part of tree is useless */
		int useless = -1;
		for (int i = 0; i < num_of_nodes; i++)
			if(min_colored_array[i] == *min_chromatic_num) {
				useless = i;
				break;
			}

		if(useless != -1)
			while(1) {
				int top_id = stack_top(stack)->id;
				if(top_id != useless) {
					node_array[top_id].color = 0;
					stack_pop(stack);
				}
				else {
					/* If I'm not zeroing color here,
					solution is different */
					node_array[top_id].color = 0;
					stack_pop(stack);
					break;
				}
			}
	}
}

/* Processes all neighbors and returns available color */
int get_color(bool* colors, Node* node, int num_of_nodes, 
	int min_chromatic_num) {
	
	/* Initialize all colors as available */
	for(int i = 1; i < num_of_nodes + 1; i++) {
		colors[i] = true;
	}

	/* Mark colors of neighbors as unavailable */
	/* Finding neighbors */
	for(int i = 0; i < num_of_nodes; i++) {

		/* Neighbor was found */
		if(graph_table[node->id][i] == true) {

			/* Set color of neighbor as unavailable, even when 
			color is 0 (none) */
			colors[node_array[i].color] = false;
		}
	}

	/* Choosing the lowest available color */
	int available_color = 0;
	/* Minimal color must be node's last color + 1, so that the node
	iterates through all its possible colors */
	/* OPTIMIZATION #1
	Also it shouldn't get color >= than saved minimal chromatic
	number, since I need only smaller solutions */
	for(int i = node->color + 1; i < min_chromatic_num; i++) {
		if(colors[i] == true) {
			available_color = i;
			break;
		}
	}

	/* available_color == 0 means this node won't get any color */
	return available_color;
}

void backtracking_csp(NodeStack *stack, int num_of_nodes) {

	/* min_chromatic_num must be bigger by 1 than num_of_nodes, so
	graphs with property colors == nodes get solution saved to array */
	int min_chromatic_num = num_of_nodes + 1;
	int *min_colored_array = calloc(num_of_nodes, sizeof(*min_colored_array));

	/* For saving colors of neighbors and then finding lowest 
	available */ 
	bool *colors = calloc(num_of_nodes + 1, sizeof(*colors));

	/* Pushing first node to stack */
	stack_push(stack, &(node_array[0]));

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
				get_color(colors, node, num_of_nodes, min_chromatic_num);

			/* Print current state of color array */
			print_coloring(min_colored_array, min_chromatic_num, NORMAL);	

			/* Node can be colored (rule can be applied) */
			if(node->color != 0) {

				/* One of goals was reached (last node in array
				was colored) */
				if(node->id == num_of_nodes - 1) { 

					success(stack, min_colored_array, num_of_nodes, 
						&min_chromatic_num);

				}
				/* Go down the tree */
				else {

					/* Take next node in node array */
					int id = (stack->top) + 1;
					stack_push(stack, &(node_array[id]));
					continue;
				}
			}
			/* Go up the tree */
			else {
				stack_pop(stack);
				continue;
			}
		}
	}

	/* Print minimal solution */
	if(print_flag)
		print_coloring(min_colored_array, min_chromatic_num, MINIMAL);
	else
		printf("%d\n", min_chromatic_num);

	free(min_colored_array);
	free(colors);
}

void fill_node(FILE* file, Node *node, int node_id, 
	int num_of_nodes) {

	/* Getting node id and initializing it */
	node->id = node_id;
	node->color = 0;

	char c;
	int position = 0;
	while((c = getc(file)) != EOF) {
		if(position > num_of_nodes) {
			fprintf(stderr, "Wrong file formatting\n");
			exit(1);
		}
		else if(isspace(c)) {
			if(c == '\n')
				break;
			else 
				continue;
		}
		else if(c == '1') {
			/* Write connection to the table */
			graph_table[node_id][position] = true;
			position++;
			continue;
		}
		else if(c == '0') {
			graph_table[node_id][position] = false;
			position++;
			continue;
		}
		else {
			fprintf(stderr, "Wrong file formatting\n");
			exit(1);
		}
	}

	if(position < num_of_nodes) {
		fprintf(stderr, "Wrong file formatting\n");
		exit(1);
	}
}

/* Gets graph info from file and creates structures representing 
this graph */
void create_graph(char* filename) {

	FILE *file;
    file = fopen(filename,"r");
    if(file == NULL) {
        fprintf(stderr, "ERROR: File opening failed\n");
        exit(-1);
    }

    /* Getting number of nodes */
    char count[100];
    fgets(count, 100, file); // reads 100 chars or line
    sscanf(count, "%d\n", &num_of_nodes);

    /* Node array where nodes are expected to be organized by id 
    for simple lookup by index of array */
	node_array = calloc(num_of_nodes, sizeof(*node_array));

    /* Empty boolean matrix representing graph */
    graph_table = calloc(num_of_nodes, sizeof(*graph_table));
    for (int i = 0; i < num_of_nodes; i++) {
    	graph_table[i] = calloc(num_of_nodes, sizeof(*graph_table[i]));
    }

    /* Getting nodes from file */
    for(int i = 0; i < num_of_nodes; i++) {
    	fill_node(file, &node_array[i], i, num_of_nodes);
    }

    fclose(file); 

    /* If graph matrix isn't symmetrical by diagonal or it contains
    self-loops, exit */
    check_matrix();

    /* Print node info */
    print_nodes(num_of_nodes);

    return;
}

int main(int argc, char* argv[]) {

	if((argc < 2 || argc > 3) || (strcmp("-h", argv[1]) == 0 || strcmp("--help", argv[1]) == 0)) {
		fprintf(stderr, "HELP: Run with './main nodes.txt [-p]',\
			\nwhere \"nodes.txt\" is file with undirected graph\
			\nrepresented by matrix and flag \"-p\" will turn on printing\
			\nof additional info about solving graph.\n");
		exit(-1);
	}
	if(argc == 3 && strcmp("-p", argv[2]) == 0)
		print_flag = true;
	else
		print_flag = false;


	/* Get graph info from file and create structures representing 
	this graph */
	create_graph(argv[1]);

    /* Seting up node stack */
    NodeStack stack;
	stack_init(&stack);
	stack.array = calloc(num_of_nodes, sizeof(*stack.array));

    /* Apply coloring algorithm on nodes and print solution */
    backtracking_csp(&stack, num_of_nodes);

    /* Freeing memory */
    for(int i = 0; i < num_of_nodes; i++) {
    	free(graph_table[i]);
    }
    free(graph_table);
    free(node_array);
    free(stack.array);

	return 0;
}

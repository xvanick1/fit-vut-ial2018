/* 

FILE NAME: 
color.c

PROJECT IDENTIFICATION: 
"6. Obarvení grafu" - substitute project from IAL course

AUTHORS:
- Josef Adamek (xadame42)
- Diana Barnova (xbarno00)
- Jozef Vanicky (xvanic09)
- Filip Weigel (xweige01)

BRIEF FILE DESCRIPTION:
Main part of coloring program, that loads graph from file, applies 
backtracking method with some specific changes on that graph, finds
coloring of graph with minimal chromatic number and prints solution

CREATED:
27.9.2018

LAST CHANGE:
27.10.2018

______________________________________________________________

NOTES:
- in this implementation self-loops (node connected to itself) 
and multiple edges between nodes are NOT allowed

TODO:
- index colors from 0 not 1
- fix solution print
- clean code for arguments
- create README
  - use make
  - program is started by ./main
  - file with graph must be in format count, matrix of connections

- headers in each program file
- create 5-10 good test graphs
- finish documentation
- error handling
- in case of error, free all allocated memory
- check if it runs on eva

- maybe try forward-checking

*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <unistd.h> // For parsing arguments

#include "stack.h" // For global variables and structures

/* For printing solution */
typedef enum {
	NORMAL,
	SMALLER,
	MINIMAL
} MODE;

/* Prints nodes info */
void print_info() {

	if(brief_flag) return;

	printf("\nNUMBER OF NODES: %d\n", num_of_nodes);
	printf("INFO: File with nodes loaded successfully. "
		"Please wait for solution...\n\n");
}

/* Prints coloring of nodes */
void print_coloring(int *min_colored_array, int min_chromatic_num, 
	int mode) {

	if(brief_flag) return;

	if(mode == NORMAL || mode == SMALLER) {

		if(mode == NORMAL) 
			printf("CHROM. NUMBER {%d}: ", min_chromatic_num);
		else 
			printf("SMALLER CHROM. NUMBER {%d}: ", min_chromatic_num);

		for (int i = 0; i < num_of_nodes; ++i) {
			if(node_array[i].color == -1)
				printf(" -");
			else
				printf(" %d", node_array[i].color);
		}
		printf("\n");
	}

	else if(mode == MINIMAL) {

		// TODO: Drop the occasional newline in printed solution
		// TODO: Change indexes to +1

		printf("|###################|###################|###################|\n");
		printf("%-20s%-20s%-20s|\n","| NODE ID", "| COLOR", "| NEIGHBORS");
		printf("|###################|###################|###################|\n");

		/* Calculate max decimal places one neighbor will take */
		int num = num_of_nodes;
		int max_dec_places = 1;
		while(num > 10) {
			num /= 10;
			max_dec_places++;
		}

		/* Max neighbors that will fit the line */
		int max_neighbors = 10 / max_dec_places;

		for(int i = 0; i < num_of_nodes; i++) {
			
			printf("| %-18d| %-18d|", i, min_colored_array[i]);

			int lenght = 0;
			char neighbor[20];

			int count = 0;
			for (int j = 0; j < num_of_nodes; j++)
			{
				if(graph_table[i][j] == true) {
					printf(" %d", j);
					sprintf(neighbor, " %d", j);
					lenght += strlen(neighbor);

					if(count >= max_neighbors) {
						printf("%*s|", 19 - lenght, "");
						printf("\n|%19s|%19s|", "", "");
						count = 0;
						lenght = 0;
					}
					else 
						count++;

				}
			}
			printf("%*s|", 19 - lenght, "");
			printf("\n|###################|###################|###################|\n");
		}

		printf("\nMINIMAL CHROMATIC NUMBER: %d\n", min_chromatic_num + 1);
	}

	else {
		fprintf(stderr, "ERROR!\n");
	}
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
	int max_color = -1; 
	for (int i = 0; i < num_of_nodes; i++) {
		if(node_array[i].color > max_color) {
			max_color = node_array[i].color;
		}
	}
	
	/* Making sure I save color array only with 
	smaller chromatic number than I already have */
	if(max_color < *min_chromatic_num) {

		*min_chromatic_num = max_color;
		for(int i = 0; i < num_of_nodes; i++) {
			min_colored_array[i] = node_array[i].color;
		}

		/* Print better solution */
		// print_coloring(min_colored_array, *min_chromatic_num, SMALLER);

		/* OPTIMIZATION #2
		Find the first node from left in min_colored_array, that has color
		same as min_chromatic_num and pop it and all neighbors next to its
		right, because this part of tree is useless for our cause */
		int useless = -1;
		for (int i = 0; i < num_of_nodes; i++)
			if(min_colored_array[i] == *min_chromatic_num) {
				useless = i;
				break;
			}

		/* Also colors of those nodes needs to be set to -1,
		because they won't get available_color = -1 in get_color() */
		if(useless != -1)

			/* Pop useless nodes and cancel their colors */
			while(1) {
				int top_id = stack_top(stack)->id;
				if(top_id != useless) {
					node_array[top_id].color = -1;
					stack_pop(stack);
				}
				/* Leftmost useless node was found */
				else {
					node_array[top_id].color = -1;
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
	for(int i = 0; i < num_of_nodes; i++) {
		colors[i] = true;
	}

	/* Mark colors of neighbors as unavailable */
	/* Finding neighbors */
	for(int i = 0; i < num_of_nodes; i++) {

		/* Neighbor was found */
		if(graph_table[node->id][i] == true) {

			/* Set color of neighbor as unavailable */
			int neighbor_color = node_array[i].color;
			if(neighbor_color != -1)
				colors[neighbor_color] = false;

		}
	}

	/* Choosing the lowest available color */
	int available_color = -1;
	/* Minimal color must be node's last color + 1, so that the node
	iterates through all its possible colors */
	/* OPTIMIZATION #1
	Also it shouldn't get color >= than saved minimal chromatic
	number, since we need only smaller solutions */
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

	/* min_chromatic_num must be at start equal to num_of_nodes, so
	graphs with property colors == nodes get solution saved to array */
	int min_chromatic_num = num_of_nodes;
	int *min_colored_array = calloc(num_of_nodes, sizeof(*min_colored_array));
	
	for(int i = 0; i < num_of_nodes; i++) {
		min_colored_array[i] = -1;
	}

	/* For saving colors of neighbors and then finding lowest 
	available */ 
	bool *colors = calloc(num_of_nodes, sizeof(*colors));

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
			// print_coloring(min_colored_array, min_chromatic_num, NORMAL);	

			/* Node can be colored (rule can be applied) */
			if(node->color != -1) {

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

	/* Print minimal solution. Since indexing of nodes and colors
	is from 0, to printed solution is added 1 for better comprehension */
	if(!brief_flag)
		print_coloring(min_colored_array, min_chromatic_num, MINIMAL);
	else
		printf("%d\n", min_chromatic_num + 1);

	free(min_colored_array);
	free(colors);
}

void fill_node(FILE* file, Node *node, int node_id, 
	int num_of_nodes) {

	/* Getting node id and initializing it */
	node->id = node_id;
	node->color = -1;

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
        exit(1);
    }

    /* Getting number of nodes */
    char count[100];
    fgets(count, 100, file); // reads 100 chars or until newline
    sscanf(count, "%d\n", &num_of_nodes);
    if(num_of_nodes <= 0) {
    	fprintf(stderr, "ERROR: Number of nodes is not valid\n");
        exit(-1);
    } 

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

    /* If graph matrix isn't symmetrical by diagonal or it contains
    self-loops, exit */
    check_matrix();
    
    fclose(file); 

    /* Print info */
    print_info();

    return;
}

char* parse_arguments(int argc, char** argv) {

	char* help = 
			"\nDESCRIPTION:\n"
			"\tProgram for finding minimal chromatic number for undirected\n"
			"\tgraph. In default mode prints info about graph and its\n"
			"\tsolution.\n"
			"\nSYNOPSIS:\n"
			"\t./main FILENAME [-h] [-b]\n"
			"\nARGUMENTS:\n"
			"\tFILENAME\n"
			"\t\tname of file with graph\n"
			"\n\t-b\n"
			"\t\toptional flag that turns on brief node,\n"
			"\t\tso program prints only minimal chromatic\n"
			"\t\tvalue of graph (used for testing)\n"
			"\n\t-h\n"
			"\t\tprint this message and exit\n"
			"\n";

	brief_flag = false;
	char *filename = NULL;

	int opt;
	while((opt = getopt(argc, argv, "bh")) != -1) {
		switch (opt) {
	        case 'b': 
	        	brief_flag = true; 
	        	break;
	        case 'h': 
	        	printf("%s", help);
		        exit(EXIT_FAILURE);
	        	break;
	        default:
	            printf("%s", help);
	            exit(EXIT_FAILURE);
        }
	}

	/* Only 1 non-option argument is allowed for filename. Function
	getopt() will put all non-option arguments to end of argv array
	and variable optind is index of first non-option argument */
	for (int i = optind; i < argc; i++) {
		if(i == optind)
			filename = argv[i];
		else {
			fprintf(stderr, "ERROR: Only 1 non-option argument is allowed\n");
			printf("%s", help);
	        exit(EXIT_FAILURE);
		}
	}

	/* Filename argument is mandatory */
	if(filename == NULL) {
		fprintf(stderr, "ERROR: Filename argument is mandatory\n");
		printf("%s", help);
        exit(EXIT_FAILURE);
    }

	return filename;
}

int main(int argc, char* argv[]) {

	/* Start measuring time */
	clock_t begin = clock();

	/* Get graph info from file and create structures representing 
	this graph */
	char* filename = parse_arguments(argc, argv);
	create_graph(filename);

    /* Set up node stack for node pointers */
    NodeStack stack;
	stack_init(&stack);
	stack.array = calloc(num_of_nodes, sizeof(*stack.array));

    /* Apply coloring algorithm on nodes and print solution */
    backtracking_csp(&stack, num_of_nodes);

    /* Free memory */
    for(int i = 0; i < num_of_nodes; i++) {
    	free(graph_table[i]);
    }
    free(graph_table);
    free(node_array);
    free(stack.array);

    /* Stop measuring time and print it */
    clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	if(!brief_flag)
		printf("MEASURED TIME: %fs\n\n", time_spent);

	return 0;
}

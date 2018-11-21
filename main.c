/* 

FILE NAME: 
color.c

PROJECT IDENTIFICATION: 
"6. Obarveni grafu" - substitute project from IAL course

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
04.11.2018

______________________________________________________________

NOTES:
- in this implementation self-loops (node connected to itself) 
and multiple edges between nodes are NOT allowed
- not gonna use atexit() since apparently on moderns systems memory
will get taken care of automatically after using exit()
- we don't have to analyze time complexity of arrays, loops or things 
like that, since that should be included to complexity of backtracking - m^n.
Although we could compare our program with complexity from wikipedia and that
one paper - 2^n*n and 2.445^n

TODO:
- should I remove stack and just use node_array and variable pointing to
current node?
- should I add heuristics (read section about CSP in BIG AI BOOK)

- headers in each program file
- create 5-10 good test graphs
- finish documentation
- at last check if it runs on eva and with valgrind
- include changed script for generating graphs to zip 

DONE
- index nodes and colors from 0 not 1
- fix solution print
- on eva -b (brief) argument is wrongly accepted
- make 2D node matrix into 1D for optimization 
- diff the two errors: file opening failed VS file not found
- num_of_nodes shouldn't exceed INT_MAX
- error handling
- create README
- first node doesn't have to go through all colors

*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <limits.h>

#include "helpers.h" // For global variables and structures

/* Makes sure only minimal solution gets saved to array */
void success(int *min_colored_array, int num_of_nodes, 
	int *min_chromatic_num, int *id) {

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

		/* OPTIMIZATION #3
		Find the first node from left in min_colored_array, that has color
		same as min_chromatic_num and cancel this branch, 
		because this part of tree is useless for our cause */
		int useless = -1;
		for (int i = 0; i < num_of_nodes; i++) {
			if(min_colored_array[i] == *min_chromatic_num) {
				useless = i;
				break;
			}
		}

		/* Also colors of those nodes needs to be set to -1,
		because they won't get available_color = -1 in get_color() */
		if(useless != -1) {

			/* Pop useless nodes and cancel their colors */
			while(1) {
				// int top_id = stack_top(stack)->id;
				int top_id = *id; // current node
				if(top_id != useless) {
					node_array[top_id].color = -1;
					// stack_pop(stack);
					(*id)--;
				}
				/* Leftmost useless node was found */
				else {
					node_array[top_id].color = -1;
					// stack_pop(stack);
					(*id)--;
					break;
				}
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
		if(graph_table[node->id * num_of_nodes + i] == true) {

			/* Set color of neighbor as unavailable */
			int neighbor_color = node_array[i].color;
			if(neighbor_color != -1)
				colors[neighbor_color] = false;
		}
	}

	/* Choosing the lowest available color */
	int available_color = -1;
	/* OPTIMIZATION #1
	First node doesn't have to go through all colors, 1 is enough */
	if(!(node->id == 0 && node->color == 0)) {
		/* OPTIMIZATION #2
		Node shouldn't get color >= than saved minimal chromatic
		number, since we need only smaller solutions */
		/* Minimal color must be node's last color + 1, so that the node
		iterates through all its possible colors */
		for(int i = node->color + 1; i < min_chromatic_num; i++) {
			if(colors[i] == true) {
				available_color = i;
				break;
			}
		}
	}

	/* available_color == 0 means this node won't get any color */
	return available_color;
}

void backtracking_csp() {

	/* min_chromatic_num must be at start equal to num_of_nodes, so
	graphs with property colors == nodes get solution saved to array */
	int min_chromatic_num = num_of_nodes;
	int *min_colored_array = calloc(num_of_nodes, sizeof(*min_colored_array));
	if(min_colored_array == NULL) {
		perror("ERROR: Memory allocation failed");
		exit(EXIT_FAILURE);
	}

	for(int i = 0; i < num_of_nodes; i++) {
		min_colored_array[i] = -1;
	}

	/* For saving colors of neighbors and then finding lowest 
	available */ 
	bool *colors = calloc(num_of_nodes, sizeof(*colors));
	if(colors == NULL) {
		perror("ERROR: Memory allocation failed");
		exit(EXIT_FAILURE);
	}

	/* Root of solution tree is first node in node_array */
	int id = 0;


	while(true) {

		/* If all color variations were tried already, stop */
		if(id == -1) {
			break;
		}
		else {

			/* Choose color for node on top of stack */ 
			Node *node = &node_array[id]; 
			node->color =
				get_color(colors, node, num_of_nodes, min_chromatic_num);

			/* Print current state of color array */
			// print_coloring(min_colored_array, min_chromatic_num, NORMAL);	

			/* Node can be colored (rule can be applied) */
			if(node->color != -1) {

				/* One of goals was reached (last node in array
				was colored) */
				if(node->id == num_of_nodes - 1) { 

					success(min_colored_array, num_of_nodes, 
						&min_chromatic_num, &id);

				}
				/* Go down the tree */
				else {

					/* Take next node in node array (push) */
					id++;
					continue;
				}
			}
			/* Go up the tree */
			else {

				/* Go 1 node back in node array (pop) */
				id--;
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

void fill_node(FILE* file, Node *node, int node_id) {

	/* Getting node id and initializing it */
	node->id = node_id;
	node->color = -1;

	char c;
	int column = 0;
	while((c = getc(file)) != EOF) {
		if(column > num_of_nodes) {
			fprintf(stderr, "ERROR: Graph matrix is not square\n");
			exit(EXIT_FAILURE);
		}
		else if(isspace(c)) {
			if(c == '\n')
				break;
			else 
				continue;
		}
		else if(c == '1') {
			/* Copy connection to the table */
			graph_table[node_id * num_of_nodes + column] = true;
			column++;
			continue;
		}
		else if(c == '0') {
			graph_table[node_id * num_of_nodes + column] = false;
			column++;
			continue;
		}
		else {
			fprintf(stderr, "ERROR: Wrong file formatting\n");
			exit(EXIT_FAILURE);
		}
	}

	if(column < num_of_nodes) {
		fprintf(stderr, "ERROR: Graph matrix is not square\n");
		exit(EXIT_FAILURE);
	}
}

/* Gets graph info from file and creates structures representing 
this graph */
void create_graph(char* filename) {

	FILE *file;
    file = fopen(filename,"r");
    if(file == NULL) {
        /* perror() will show message based on set errno */
        perror("ERROR: File opening failed");
        exit(EXIT_FAILURE);
    }

    /* Getting number of nodes */
    char max_num_len[11]; // 10 places is for INT_MAX value, 1 for '\0'
    /* fgets() reads 10 chars or until newline */
    if(fgets(max_num_len, 11, file) == NULL) {
    	fprintf(stderr, "ERROR: fgets() failed to read number of nodes\n");
    	exit(EXIT_FAILURE);
    }
    sscanf(max_num_len, "%d\n", &num_of_nodes);
    if(num_of_nodes <= 0 || num_of_nodes > INT_MAX) {
    	fprintf(stderr, "ERROR: Number of nodes is not valid - must be "
    		"in interval <1; %d>\n", INT_MAX);
        exit(EXIT_FAILURE);
    } 

    /* Node array where nodes are expected to be organized by id 
    for simple lookup by index of array */
	node_array = calloc(num_of_nodes, sizeof(*node_array));
	if(node_array == NULL) {
		perror("ERROR: Memory allocation failed");
		exit(EXIT_FAILURE);
	}

    /* Empty boolean array representing matrix representing graph */
    graph_table = calloc(num_of_nodes * num_of_nodes, sizeof(*graph_table));
	if(graph_table == NULL) {
		perror("ERROR: Memory allocation failed");
		exit(EXIT_FAILURE);
	}

    /* Getting nodes from file */
    for(int i = 0; i < num_of_nodes; i++) {
    	fill_node(file, &node_array[i], i);
    }

    /* If graph matrix isn't symmetrical by diagonal or it contains
    self-loops, exit */
    check_matrix();
    
    /* Print info */
    if(!brief_flag) print_info();

    fclose(file); 

    return;
}


int main(int argc, char* argv[]) {

	/* Get graph info from file and create structures representing 
	this graph */
	char* filename = parse_arguments(argc, argv);
	create_graph(filename);

	/* Start measuring time */
	clock_t begin = clock();

    /* Apply coloring algorithm on nodes and print solution */
    backtracking_csp();

    /* Stop measuring time */
    clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

    /* Free memory */
    free(graph_table);
    free(node_array);

    /* Print measured time */
	if(!brief_flag) {

		/* Prints time on 4 decimal */
		if (time_spent < 0.000100) {
			printf("APPROXIMATED MEASURED TIME: %fs\n\n", time_spent);
		}
		/* Prints time in minutes and seconds if algorhitm lasts longer than a minute */
		else if (time_spent >= 60.000000) {
			int mins = (time_spent/60);
			float secs = time_spent-(mins*60);
			printf("APPROXIMATED MEASURED TIME: %dmin and %0.0fs\n\n", mins, secs);
		}
		else {
			printf("APPROXIMATED MEASURED TIME: %0.4fs\n\n", time_spent);
		}
		return 0;
	}
}

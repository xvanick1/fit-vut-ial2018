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

int print_colors() {
	// printf("Nodes : ");
	// for(int i = 0; i < num_of_nodes; i++) {
	// 	printf("%d ",i);
	// }
	// printf("\n");
	// printf("Colors: ");
	// for(int i = 0; i < num_of_nodes; i++) {
	// 	int color = node_array[i].color;
	// 	if(color == -1)
	// 		printf("- ");
	// 	else
	// 		printf("%d ",node_array[i].color);
	// }
	// printf("\n");
    int max_color = -1; 
	for (int i = 0; i < num_of_nodes; i++) {
		if(node_array[i].color > max_color) {
			max_color = node_array[i].color;
		}
	}
	return max_color;
}

void print_sets(int i) {
	printf("Node %d color: %d\n",i,node_array[i].color);
	for(int j = 0; j < num_of_nodes; j++) {
		printf("Node %d = {",j);
		for(int k = 0; k < num_of_nodes; k++) {
			if(node_array[j].color_set[k]) {
				printf("%d ", k);
			}
		}
		printf("}\n");
	}
}

/* Delete from colorsets all colors that are in conflict with assigned 
values */
void delete_conflicting_colors(int i) {

	for(int j = i + 1; j < num_of_nodes; j++) { // j is node id
			
		/* Neighbor was found */
		if(graph_table[j * num_of_nodes + i] == true) {

			/* Check if there is conflict between nodes colors,
			and if so, drop that color from the set */
			for(int k = 0; k < num_of_nodes; k++) { // k is color in color set
				
				/* Need to check constrains only with i-th node */
				int ith_color = node_array[i].color;
				int jth_color = node_array[j].color_set[k];

				if(jth_color == true && k == ith_color) {
					// printf("Conflict between nodes %d and %d: color %d\n",k,j,l); 
					node_array[j].color_set[k] = false;
				}
			}
		}
	}
}

/* Figuring out if some colorset of nodes from j to n is empty */
bool is_some_jth_colorset_empty(int i) {

	for(int j = i + 1; j < num_of_nodes; j++) { // j is node id
		bool empty = true;
		
		/* Here I go through all colors in color set of node j
		and check if some set is empty */
		for(int k = 0; k < num_of_nodes; k++) { // k is color in color_set
			if(node_array[j].color_set[k] == true) {
				empty = false;
			}
		}
		if(empty == true) {
			// printf("Fail: Colorset of node %d is empty\n",j);
			return true;
		}
	}
	return false;
}

/* Going through the i-th node's color set and finding out, 
if it's empty */
bool is_ith_node_colorset_empty(int i) {
	bool empty = true;
	for(int j = 0; j < num_of_nodes; j++) {
		if(node_array[i].color_set[j] == true) {
			empty = false;
		}
	}
	return empty;
}

void rollback_colorsets(int i, bool **rollback_array) {
	for(int j = i + 1; j < num_of_nodes; j++) {
		/* The "i" instead of "num_of_nodes" is debatable */
		/* Actually I think this should be optimized later */
		for(int k = 0; k < i+1; k++) {
			/* Change things back using goback arrays */
			if(rollback_array[j][k] == true) {
				// printf("Putting back color %d to colorset of node %d\n",k,j);
				node_array[j].color_set[k] = true;
			}
		}
	}
}

void free_rollback_array(int i, bool **rollback_array) {
	for(int j = i+1; j < num_of_nodes; j++) {
		free(rollback_array[j]);
	}
	free(rollback_array);
}

/* Iterate through nodes and copy their colorsets */
void copy_colorsets_for_rollback(int i, bool **rollback_array) {
	for(int j = i+1; j < num_of_nodes; j++) {

		/* Purposefully filling array with zeroes so that all bools 
		are initialized to false */
		rollback_array[j] = calloc(num_of_nodes, sizeof(**rollback_array));
		
		/* Iterate through their bool arrays */
		for(int k = 0; k < num_of_nodes; k++) {
			rollback_array[j][k] = node_array[j].color_set[k];
		}
	}
}

void pop_first_color_and_assign_it(int i) {
	for(int j = 0; j < num_of_nodes; j++) {
		if(node_array[i].color_set[j] == true) {
			node_array[i].color = j;
			node_array[i].color_set[j] = false;
			break;
		}
	}
}

bool forward_checking(int i) {

	/* Save colorsets of nodes from i+1 to n for use in case of rollback */
	/* Pointer to array of arrays of bools */
	bool **rollback_array = calloc(num_of_nodes, sizeof(*rollback_array));
		
	/* Iterate through nodes and copy their colorsets */
	copy_colorsets_for_rollback(i, rollback_array);

	while(42) {

		/* Delete first color from color set of first node 
		and assign it to it */
		pop_first_color_and_assign_it(i);

		// printf("\n");
		// print_colors();
		// print_sets(i);
		
		/* If wanted coloring is found, end */
		if(i == num_of_nodes - 1) {

			/* Free alocated memory of node sets */
			free_rollback_array(i, rollback_array);
			return true;
		}

		/* Delete from sets all colours that are in conflict with assigned 
		values */
		delete_conflicting_colors(i);

		/* If some colorset of nodes from j to n is empty, roll back */
		if(is_some_jth_colorset_empty(i)) {
			rollback_colorsets(i, rollback_array);
		}
		else {

			/* Recursive calling */
			if(forward_checking(i + 1)) {
				/* Free alocated memory of node sets */
				free_rollback_array(i, rollback_array);
				return true;
			}
			else {
				/* Rollback after comeback from recursive calling */
				rollback_colorsets(i, rollback_array);
			}
		}

		/* Going through the i-th node's color set and finding out, 
		if it's empty */
		if(is_ith_node_colorset_empty(i)) {

			/* Giving nodes clean slate */
			node_array[i].color = -1;

			/* Free alocated memory of node sets */
			free_rollback_array(i, rollback_array);
			return false;
		}
		else {
			continue;
		}
	}

	
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

    /* Allocating memory for color sets of nodes */
    for(int i = 0; i < num_of_nodes; i++) {
    	// *(node_array[i].color_set)
    	node_array[i].color_set = calloc(num_of_nodes, sizeof(bool));
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
	for(int round = 0; round < num_of_nodes; round++) {

	    /* I need to make colorsets true only to the number of the 
	    round */
	    for(int j = 0; j < num_of_nodes; j++) {

	    	for(int k = 0; k < round + 1; k++) {
		    	node_array[j].color_set[k] = true;
	    	}

	    	/* Also need to erase numbers from round before */
	    	node_array[j].color = -1;
		}
		if(forward_checking(0)) {
			// printf("RESULT: %s\n", "SUCCESS");
		    if(brief_flag)
			    printf("%d", print_colors() + 1);
			else
			    print_coloring(print_colors(), MINIMAL);
			break;
		}
		else {
			// printf("RESULT: %s\n", "FAILURE");
			// print_colors();
		}
	}

    /* Stop measuring time */
    clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

    /* Free memory */
    free(graph_table);
    for(int i = 0; i < num_of_nodes; i++) {
    	free(node_array[i].color_set);
    }
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

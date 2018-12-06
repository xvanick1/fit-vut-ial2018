/********************************************************************

FILE NAME: 
main.c

PROJECT IDENTIFICATION: 
Subject: IAL
Number: 6
Name: Obarveni grafu

ZADANI:
Obarvenim grafu rozumime prirazeni barev uzlum grafu, 
pricemz zadne dva sousedni uzly nesmi byt obarveny stejne. 
Minimalni pocet pouzitych barev se nazyva chromaticke cislo.
Vytvorte program pro hledani minimalniho obarveni neorientovanych 
grafu.
Pokud existuje vice reseni, staci nalezt pouze jedno. Vysledky 
prezentujte vhodnym zpusobem. Soucasti projektu bude nacitani 
grafu ze souboru a vhodne testovaci grafy. 
V dokumentaci uvedte teoretickou slozitost ulohy 
a porovnejte ji s experimentalnimi vysledky. 

AUTHORS:
- Josef Adamek (xadame42)
- Diana Barnova (xbarno00)
- Jozef Vanicky (xvanic09)
- Filip Weigel (xweige01)

BRIEF FILE DESCRIPTION:
Main part of coloring program, that loads graph from file, applies 
forward checking algorithm, finds minimal coloring of graph (chromatic
number) with minimal chromatic number and prints nodes with colors
assigned to them

CREATED:
27.9.2018

LAST CHANGE:
05.12.2018

********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <limits.h>

/* For global variables, structures and helping functions */
#include "helpers.h"

/* Goes through all colors assigned to nodes and finds 
chromatic number of graph */
int get_chromatic_number() {

    int max_color = -1; 
	for (int id = 0; id < num_of_nodes; id++) {
		if(node_array[id].color > max_color) {
			max_color = node_array[id].color;
		}
	}
	return max_color;
}

/* Delete from colorsets of nodes i+1 all colors that are in conflict 
with assigned values */
void delete_conflicting_colors(int i) {

	for(int id = i + 1; id < num_of_nodes; id++) { 
			
		/* Neighbor was found */
		if(graph_matrix[id * num_of_nodes + i] == true) {

			/* Check if there is conflict between nodes colors,
			and if so, drop that color from the set */
			int check_color = node_array[i].color;
			if(node_array[id].color_set[check_color]) {
				node_array[id].color_set[check_color] = false;
			}				
		}
	}
}

/* Figuring out if some colorset of nodes from j to n is empty */
bool is_some_i_plus_one_colorset_empty(int i) {

	for(int id = i + 1; id < num_of_nodes; id++) {
		bool empty = true;
		
		/* Here I go through all colors in color set of node j
		and check if some set is empty */
		for(int color = 0; color < num_of_nodes; color++) {
			if(node_array[id].color_set[color] == true) {
				empty = false;
			}
		}
		if(empty == true) {
			return true;
		}
	}
	return false;
}

/* Going through the i-th node's color set and finding out, 
if it's empty */
bool is_ith_node_colorset_empty(int i) {
	bool empty = true;
	for(int id = 0; id < num_of_nodes; id++) {
		if(node_array[i].color_set[id] == true) {
			empty = false;
		}
	}
	return empty;
}

/* Give nodes back colors from point when they first accessed this
level of recursion of algorithm */
void rollback_colorsets(int i, bool **rollback_array) {
	for(int id = i + 1; id < num_of_nodes; id++) {

		/* I have to go through only i colors because at i-th level
		I need only i colors anyway */
		for(int color = 0; color < i+1; color++) {
			/* Change things back using goback arrays */
			if(rollback_array[id][color] == true) {
				node_array[id].color_set[color] = true;
			}
		}
	}
}

/* When going back from recursive calling, memory allocated for 
rollback array needs to be freed */ 
void free_rollback_array(int i, bool **rollback_array) {
	for(int id = i+1; id < num_of_nodes; id++) {
		free(rollback_array[id]);
	}
	free(rollback_array);
}

/* Iterate through nodes and copy their colorsets */
void copy_colorsets_for_rollback(int i, bool **rollback_array) {
	for(int id = i+1; id < num_of_nodes; id++) {

		/* Purposefully filling array with zeroes so that all bools 
		are initialized to false */
		rollback_array[id] = calloc(num_of_nodes, sizeof(**rollback_array));
		
		/* Iterate through their bool arrays */
		for(int color = 0; color < num_of_nodes; color++) {
			rollback_array[id][color] = node_array[id].color_set[color];
		}
	}
}

/* Function will delete color from colorset of node and will assign
it to that node */
void pop_first_color_and_assign_it(int i) {
	for(int color = 0; color < num_of_nodes; color++) {
		if(node_array[i].color_set[color] == true) {
			node_array[i].color = color;
			node_array[i].color_set[color] = false;
			break;
		}
	}
}

/* Algorithm for finding coloring of graph using forward checking */
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
		
		/* If wanted coloring is found, end */
		if(i == num_of_nodes - 1) {

			/* Free alocated memory of node sets */
			free_rollback_array(i, rollback_array);
			return true;
		}

		/* Don't assign to first node bigger color than 0. If it gets 
		assigned, it means solution for that number of colors doesn't
		exist */
		if(node_array[0].color > 0) {
			/* Free alocated memory of rollback array */
			free_rollback_array(i, rollback_array);
			return false;
		}

		/* Delete from sets all colours that are in conflict with assigned 
		values */
		delete_conflicting_colors(i);

		/* If some colorset of nodes from j to n is empty, roll back */
		if(is_some_i_plus_one_colorset_empty(i)) {
			rollback_colorsets(i, rollback_array);
		}
		else {

			/* Recursive calling */
			if(forward_checking(i + 1)) {
				/* Free alocated memory of rollback array */
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

			/* Free alocated memory of rollback array */
			free_rollback_array(i, rollback_array);
			return false;
		}
		else {
			continue;
		}
	}
}

/* Function will initialize node with values and moves its connestions 
to neighbors from file to graph represented also by matrix of connections */
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
			/* Copy connection to the matrix */
			graph_matrix[node_id * num_of_nodes + column] = true;
			column++;
			continue;
		}
		else if(c == '0') {
			graph_matrix[node_id * num_of_nodes + column] = false;
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
    graph_matrix = calloc(num_of_nodes * num_of_nodes, sizeof(*graph_matrix));
	if(graph_matrix == NULL) {
		perror("ERROR: Memory allocation failed");
		exit(EXIT_FAILURE);
	}

    /* Getting nodes from file */
    for(int id = 0; id < num_of_nodes; id++) {
    	fill_node(file, &node_array[id], id);

    	/* Allocating memory for color sets of nodes */
    	node_array[id].color_set = calloc(num_of_nodes, 
    		sizeof(*(node_array[id].color_set)));
    }

    /* If graph matrix isn't symmetrical by diagonal or it contains
    self-loops, exit */
    check_matrix();
    
    /* Print info */
    if(!brief_flag) print_info();

    fclose(file);

    return;
}

/* Main function parses arguments, creates structures representing 
undirected graph, fills them with data gotten from file,
runs algorithm for finding solution with chromatic number of this graph
and then it prints solution */
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
	    for(int id = 0; id < num_of_nodes; id++) {

	    	for(int color = 0; color < round + 1; color++) {
		    	node_array[id].color_set[color] = true;
	    	}

	    	/* Also need to erase numbers from round before */
	    	node_array[id].color = -1;
		}
		if(forward_checking(0)) {
			int min_chrom_num = get_chromatic_number(); // starting at 0
		    if(brief_flag)
			    printf("%d\n", min_chrom_num + 1);
			else
			    print_coloring(min_chrom_num, MINIMAL);
			break;
		}
	}

    /* Stop measuring time */
    clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

    /* Free memory */
    free(graph_matrix);
    for(int id = 0; id < num_of_nodes; id++) {
    	free(node_array[id].color_set);
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

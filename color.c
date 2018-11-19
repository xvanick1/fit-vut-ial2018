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

TODO:
- create README

- headers in each program file
- create 5-10 good test graphs
- finish documentation
- at last check if it runs on eva and with valgrind

DONE
- index nodes and colors from 0 not 1
- fix solution print
- on eva -b (brief) argument is wrongly accepted
- make 2D node matrix into 1D for optimization 
- diff the two errors: file opening failed VS file not found
- num_of_nodes shouldn't exceed INT_MAX
- error handling

*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <unistd.h> // For parsing arguments
#include <limits.h>

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

/* Prints coloring of nodes and their neighbors. For better comprehension
indexes of nodes, colors and chromatic number are raised by 1 */
void print_coloring(int *min_colored_array, int min_chromatic_num, 
	int mode) {

	if(brief_flag) return;

	if(mode == NORMAL || mode == SMALLER) {

		if(mode == NORMAL) 
			printf("CHROM. NUMBER {%d}: ", min_chromatic_num + 1);
		else 
			printf("SMALLER CHROM. NUMBER {%d}: ", min_chromatic_num + 1);

		for (int i = 0; i < num_of_nodes; ++i) {
			if(node_array[i].color == -1)
				printf(" -");
			else
				printf(" %d", node_array[i].color);
		}
		printf("\n");
	}

	else if(mode == MINIMAL) {

		printf("+———————————————————+———————————————————+———————————————————+\n");
		printf("%-20s%-20s%-20s|\n","| NODE ID", "| COLOR", "| NEIGHBORS");
		printf("+———————————————————+———————————————————+———————————————————+\n");

		for(int i = 0; i < num_of_nodes; i++) {
			
			/* Print node id and color */
			printf("| %-18d| %-18d|", i + 1, min_colored_array[i] + 1);

			int max_chars_on_line = 18;

			int lenght = 0; // length of neighbors on line
			char neighbor[20];

			for (int j = 0; j < num_of_nodes; j++)
			{
				if(graph_table[i * num_of_nodes + j] == true) {

					/* Get char lenght of neighbor */
					sprintf(neighbor, " %d", j + 1);
					int neighbor_len = strlen(neighbor);

					/* Neighbor won't fit on line, so it's needed to 
					print it on next */
					if(lenght + neighbor_len > max_chars_on_line) {
						printf("%*s|", 19 - lenght, "");
						printf("\n|%19s|%19s|", "", "");

						printf(" %d", j + 1);
						lenght = neighbor_len;
					}
					/* Neighbor fits the line */
					else {
						printf(" %d", j + 1);
						lenght += neighbor_len;
					}
				}
			}

			printf("%*s|", 19 - lenght, "");
			printf("\n+———————————————————+———————————————————+———————————————————+\n");
		}

		printf("\nMINIMAL CHROMATIC NUMBER: %d\n", min_chromatic_num + 1);
	}
}

/* Parses arguments with getopt() function */
char* parse_arguments(int argc, char** argv) {

	char* help = 
			"\nDESCRIPTION:\n"
			"\tProgram for finding minimal chromatic number for undirected\n"
			"\tgraph. In default mode prints info about graph and its\n"
			"\tsolution.\n"
			"\nSYNOPSIS:\n"
			"\t./main -f FILENAME [-h] [-b]\n"
			"\nARGUMENTS:\n"
			"\t-f FILENAME\n"
			"\t\tfile flag followed by name of file with graph,\n"
			"\t\tallowed file extensions are .txt, .in or none\n"
			"\n\t-b\n"
			"\t\toptional flag that turns on brief node,\n"
			"\t\tso that program prints only minimal chromatic\n"
			"\t\tvalue of graph (used for testing with -f flag)\n"
			"\n\t-h\n"
			"\t\tprint this message and exit\n"
			"\n";

	brief_flag = false;
	char *filename = NULL;

	/* To turn off getopt() printed errors */
	opterr = 0;

	/* Parse arguments from command line */
	int opt;
	while((opt = getopt(argc, argv, "f:bh")) != -1) {
		switch (opt) {
			case 'f':
				filename = optarg;
				break;
	        case 'b': 
	        	brief_flag = true; 
	        	break;
	        case 'h': 
	        	printf("%s", help);
		        exit(EXIT_SUCCESS);
	        	break;

	        case '?':
	        	if(optopt == 'f')
	        		fprintf(stderr, "ERROR: Option -%c requires an argument\n", optopt);
	        	else if(isprint(optopt))
	        		fprintf(stderr, "ERROR: Unknown option -%c\n", optopt);
	        	else 
	        		fprintf (stderr, "ERROR: Unknown option character `\\x%x'\n", optopt);
	        	
	        	printf("%s", help);
	            exit(EXIT_FAILURE);

	        default:
	            printf("%s", help);
	            exit(EXIT_FAILURE);
        }
	}

	/* Unknown non-option arguments are not allowed */
	for (int i = optind; i < argc; i++) {
	    fprintf(stderr, "ERROR: Unknown non-option argument '%s'\n", argv[i]);
	    printf("%s", help);
	    exit(EXIT_FAILURE);
	}

	/* Filename argument is mandatory */
	if(filename == NULL) {
		fprintf(stderr, "ERROR: Filename argument is mandatory\n");
		printf("%s", help);
        exit(EXIT_FAILURE);
    }

    /* Checking if file has right extension - none, .txt or .in */
    bool dotflag = false;
    char ext[4] = {0};
    int count = 0;
    for(int i = 0; i < strlen(filename); i++) {

    	/* Checking dot */
    	if(filename[i] == '.' && dotflag == false) {
    		dotflag = true;
    		continue;
    	}

    	/* Loading the extension */
    	if(dotflag == true && count < 3) {
    		ext[count] = filename[i];
    		count++;
    	}
    }

    if(dotflag) {
    	int txt = strcmp(ext,"txt");
    	int in = strcmp(ext,"in");
	    if(!((txt == 0 && in != 0) || (txt != 0 && in == 0))) {
	    	fprintf(stderr, "ERROR: Bad file extension (allowed: .txt, .in or none)\n");
	    	exit(EXIT_FAILURE);
	    }
    }

	return filename;
}

/* Checks if matrix representing graph is symmetrical by diagonal
and that it doesn't contain self-loops. Otherwise it ends program */
void check_matrix() {
	int diagonal = 0;
	for (int i = 0; i < num_of_nodes; i++)
	{
		if(graph_table[i * num_of_nodes + diagonal] == 1) {
			fprintf(stderr, "ERROR: Self-loops are not allowed\n");
			exit(EXIT_FAILURE);
		}
		for (int j = diagonal; j < num_of_nodes; j++)
			if(graph_table[i * num_of_nodes + j] != 
				graph_table[j * num_of_nodes + i]) 
			{
				fprintf(stderr, "ERROR: Input graph is not undirected\n");
				exit(EXIT_FAILURE);
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
		if(graph_table[node->id * num_of_nodes + i] == true) {

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

	/* Pushing first node to stack */
	stack_push(stack, &(node_array[0]));

	while(true) {

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
    	fill_node(file, &node_array[i], i, num_of_nodes);
    }

    /* If graph matrix isn't symmetrical by diagonal or it contains
    self-loops, exit */
    check_matrix();
    
    /* Print info */
    print_info();

    fclose(file); 

    return;
}


int main(int argc, char* argv[]) {

	/* Get graph info from file and create structures representing 
	this graph */
	char* filename = parse_arguments(argc, argv);
	create_graph(filename);

    /* Set up node stack for node pointers */
    NodeStack stack;
	stack_init(&stack);
	stack.array = calloc(num_of_nodes, sizeof(*stack.array));
	if(stack.array == NULL) {
		perror("ERROR: Memory allocation failed");
		exit(EXIT_FAILURE);
	}

	/* Start measuring time */
	clock_t begin = clock();

    /* Apply coloring algorithm on nodes and print solution */
    backtracking_csp(&stack, num_of_nodes);

    /* Stop measuring time */
    clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

    /* Free memory */
    free(graph_table);
    free(node_array);
    free(stack.array);

    /* Print measured time */
	if(!brief_flag) {

		/* Prints time on 4 decimal */
		if (time_spent < 0.000100) {
			printf("APROXIMATE MEASURED TIME: %fs\n\n", time_spent);
		}
		/* Prints time in minutes and seconds if algorhitm lasts longer than a minute */
		else if (time_spent >= 60.000000) {
			int mins = (time_spent/60);
			float secs = time_spent-(mins*60);
			printf("APROXIMATE MEASURED TIME: %dmin and %0.0fs\n\n", mins, secs);
		}
		else {
			printf("APROXIMATE MEASURED TIME: %0.4fs\n\n", time_spent);
		}
		return 0;
	}
}

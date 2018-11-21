/*
In this file there are helper functions
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h> // For parsing arguments

#include "helpers.h"

/* Prints nodes info */
void print_info() {
	printf("\nNUMBER OF NODES: %d\n", num_of_nodes);
	printf("INFO: File with nodes loaded successfully. "
		"Please wait for solution...\n\n");
}

/* Prints coloring of nodes and their neighbors. For better comprehension
indexes of nodes, colors and chromatic number are raised by 1 */
void print_coloring(int *min_colored_array, int min_chromatic_num, 
	int mode) {

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
    	bool txt = (strcmp(ext,"txt") == 0);
    	bool in = (strcmp(ext,"in") == 0);
    	if(!txt && !in) {
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

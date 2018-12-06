/********************************************************************

FILE NAME: 
helpers.h

PROJECT IDENTIFICATION: 
Subject: IAL
Number: 6
Name: Obarveni grafu

AUTHORS:
- Josef Adamek (xadame42)
- Diana Barnova (xbarno00)
- Jozef Vanicky (xvanic09)
- Filip Weigel (xweige01)

BRIEF FILE DESCRIPTION:
This file contains structures representing undirected graph (node structure,
array of nodes and matrix of connections)declarations of functions from 
file helpers.c and global variables used in both main.c and helpers.c

CREATED:
27.9.2018

LAST CHANGE:
05.12.2018

********************************************************************/

#include <stdbool.h>

/* Global variables */
bool brief_flag;
int num_of_nodes;

/* Enumerating codes for printing solution */
typedef enum {
	NORMAL,
	SMALLER,
	MINIMAL
} MODE;

/* Structure for node in graph */
typedef struct {
	int id; // id of node equals index in node_array
	int color;
	bool *color_set;
} Node;

/* Heavily used data structures */
bool *graph_matrix;
Node *node_array;

/* Declarations of functions in helpers.c */
void print_info();

void print_coloring(int min_chromatic_num, 
	int mode);

char* parse_arguments(int argc, char** argv);

void check_matrix();

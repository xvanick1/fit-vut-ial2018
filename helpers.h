#include <stdbool.h>

bool brief_flag;

int num_of_nodes; // used in stack_full

/* For printing solution */
typedef enum {
	NORMAL,
	SMALLER,
	MINIMAL
} MODE;

typedef struct {
	int id; // id of node equals index in node_array
	int color;
	bool *color_set;
	bool *goback_array;
} Node;

/* Heavily used data types */
bool *graph_table;
Node *node_array;

void print_info();

void print_coloring(int *min_colored_array, int min_chromatic_num, 
	int mode);

char* parse_arguments(int argc, char** argv);

void check_matrix();
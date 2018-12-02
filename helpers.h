#include <stdbool.h>

bool brief_flag;

int num_of_nodes; // used in stack_full

/* For printing solution */
typedef enum {
	NORMAL,
	SMALLER,
	MINIMAL
} MODE;


/* Color list */
typedef struct color_elem {
	int color;
	struct color_elem *left_ptr;
	struct color_elem *right_ptr;
} ColorElement; 

typedef struct {
	ColorElement *first;
	ColorElement *last;
	ColorElement *active;
} ColorList;

typedef struct {
	int id; // id of node equals index in node_array
	int color;
	// bool *color_set;
	ColorList color_set;
} Node;

/* Node list */
// typedef struct elem {
// 	int id;
// 	int color;
// 	// ColorList list;
// 	struct elem *next_ptr;
// } Node;

// typedef struct {
// 	NodeElement *first;
// 	NodeElement *active;
// } NodeList;

/* Heavily used data types */
bool *graph_table;
Node *node_array;

/* Declarations of helper functions */
void print_info();

void print_coloring(int *min_colored_array, int min_chromatic_num, 
	int mode);

char* parse_arguments(int argc, char** argv);

void check_matrix();

/* Declarations of double list functions */
void list_init(ColorList *list);

void insert_last(ColorList *list, int color);

void make_first_active (ColorList *list);

int copy_active (ColorList *list);

int copy_first(ColorList *list);

void make_next_active(ColorList *list);

void delete_first(ColorList *list);

void delete_list(ColorList *list);

bool is_list_active(ColorList *list);

bool find_color(ColorList *list, int color);

void copy_list(ColorList *list, ColorList *copy);

void delete_active(ColorList *list);

void print_list(ColorList *list, int node_id);


int num_of_nodes; // used in stack_full

typedef struct {
	int id; // id of node equals index in node_array
	int color;
	int num_of_neighbors;
	int* neighbors;
} Node;

/* Stack of pointers to nodes, that are actually in node_array */
typedef struct {
	int top; // Index of top node
	Node** array; // Working with pointers to nodes is more effective maybe
} NodeStack;

void stack_init(NodeStack *stack);

int stack_empty(const NodeStack *stack);

int stack_full(const NodeStack *stack);

Node* stack_top(const NodeStack *stack);

void stack_pop(NodeStack *stack);

void stack_push(NodeStack *stack, Node *node);

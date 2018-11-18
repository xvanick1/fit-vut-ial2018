/*
Stack construct in this file was strongly inspired 
by 3rd IAL presentation
*/

#include <stdio.h>
#include <stdlib.h>

#include "stack.h"

/* Prints warning about error while working with stack end exits */
void stack_error(char* error_string) {
	printf("%s\n", error_string);
	exit(EXIT_FAILURE);
}

/* Will set top of stack */
void stack_init(NodeStack *stack) {

	if(stack == NULL) {
		stack_error("STACK ERROR: Init");
		return;
	}

	/* top == -1 stands for empty stack */
	stack->top = -1;
}

/* Returns nonzero value if stack is empty, else zero */
int stack_empty(const NodeStack *stack) {
	return stack->top == -1;
}

/* Returns nonzero value if stack is full, else zero */
int stack_full(const NodeStack *stack) {
	return stack->top == num_of_nodes - 1;
}

/* Returns pointer to node from top of the stack */
Node* stack_top(const NodeStack *stack) {

	if (stack_empty(stack)) {
		stack_error("STACK ERROR: Top");
		return NULL;
	}

	return stack->array[stack->top];
}

/* Removes node from top of the stack (decreases top by 1) */
void stack_pop(NodeStack *stack) {

	if (!stack_empty(stack)) {
		stack->top--;
	}
	else {
		stack_error("STACK ERROR: Pop");
	}
}

/* Puts node on top of stack */
void stack_push(NodeStack *stack, Node *node) {

	if (stack_full(stack)) {
		stack_error("STACK ERROR: Push");
		return;
	}

	/* This means it increases top by 1, will write on that
	index of array and will overwrite node pointer,
	if there is one */
	stack->top++;
	stack->array[stack->top] = node;
    
}

#include <stdio.h>
#include <stdlib.h>

#include "stack.h"

#define MAX_SERR 4

typedef enum {
	SERR_INIT = 1,
	SERR_TOP = 2,
	SERR_POP = 3,
	SERR_PUSH = 4
} STACK_ERROR;

int error_flag; 

/* Prints warning about error while working with stack */
void stack_error(int error_code)
{
	static const char *SERR_STRINGS[MAX_SERR + 1] = {
		"Unknown error",
		"Stack error: INIT",
		"Stack error: TOP",
		"Stack error: POP",
		"Stack error: PUSH",
	};

	if (error_code <= 0 || error_code > MAX_SERR)
	{
		error_code = 0;
	}

	printf("%s\n", SERR_STRINGS[error_code]);
	error_flag = 1;
}


/* Will set top of stack */
void stack_init(NodeStack *stack) {

	if(stack == NULL) {
		stack_error(SERR_INIT);
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

/* Returns id of node from top of the stack */
Node* stack_top(const NodeStack *stack) {

	if (stack_empty(stack))
	{
		stack_error(SERR_TOP);
		return NULL;
	}

	return stack->array[stack->top];
}

/* Removes node from top of the stack (decreases top by 1) */
void stack_pop(NodeStack *stack) {

	if (!stack_empty(stack))
	{
		stack->top--;
	}
	else {
		stack_error(SERR_POP);
	}
}

/* Puts node on top of stack */
void stack_push(NodeStack *stack, Node *node) {

	if (stack_full(stack))
	{
		stack_error(SERR_PUSH);
		return;
	}

	/* This means it increases top by 1, will write on that
	index of array and will overwrite node pointer,
	if there is one */
	stack->top++;
	stack->array[stack->top] = node;
    
}

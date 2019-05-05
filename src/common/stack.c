#include "stack.h"
#include <stdlib.h>

void Stack_Init(Stack *s) {
	s->index = -1;
}

uint8_t Stack_Push(Stack *s, void *data) {
	/* If stack capacity has been reached, exit */
	if ((s->index + 1) >= MAX_STACK)
		return EXIT_FAILURE;

	s->index++;
	s->data[s->index] = data;
	return EXIT_SUCCESS;
}

void* Stack_Pop(Stack *s) {
	/* If stack is on bottom, exit */
	if (s->index < 0)
		return NULL;

	s->index--;
	return s->data[s->index + 1];
}

uint8_t Stack_IsEmpty(Stack *s) {
	return (s->index == -1);
}

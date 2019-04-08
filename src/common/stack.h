#ifndef STACK_H
#define STACK_H

#define MAX_STACK 20

#include "stdint.h"

typedef struct {
	void *data[MAX_STACK];
	int8_t index;
} Stack;

/**
 * \fn Stack_Init
 *
 * \param s instance of Stack
 */
void Stack_Init(Stack *s);

/**
 * \fn Stack_Push
 *
 * \param s instance of Stack
 * \param data instance of data
 *
 * \return EXIT_SUCCESS if succeed, EXIT_FAILURE otherwise
 */
uint8_t Stack_Push(Stack *s, void *data);

/**
 * \fn Stack_Pop
 *
 * \param s instance of Stack
 *
 * \return instance of data popped
 */
void* Stack_Pop(Stack *s);

/**
 * \fn Stack_IsEmpty
 *
 * \param s instance of Stack
 *
 * \return 1 if stack empty, 0 otherwise
 */
uint8_t Stack_IsEmpty(Stack *s);

#endif /* STACK_H */

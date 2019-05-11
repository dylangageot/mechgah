/**
 * \file stack.h
 * \brief header file of Stack module
 * \author Dylan Gageot
 * \version 
 * \date 2019-05-11
 *
 * Implementation of a FILO stack
 */

#ifndef STACK_H
#define STACK_H

/**
 * \brief Max capacity of Stack set to 20
 */
#define MAX_STACK 20

#include "stdint.h"

/**
 * \brief Hold stack data and capacity information
 */
typedef struct {
	void *data[MAX_STACK];
	int8_t index;
} Stack;

/**
 * \brief Initialize given Stack
 * \param s instance of Stack
 */
void Stack_Init(Stack *s);

/**
 * \brief Push data to Stack
 *
 * \param s instance of Stack
 * \param data instance of data
 *
 * \return EXIT_SUCCESS if succeed, EXIT_FAILURE otherwise
 */
uint8_t Stack_Push(Stack *s, void *data);

/**
 * \brief Pop data from Stack
 *
 * \param s instance of Stack
 *
 * \return instance of data popped
 */
void* Stack_Pop(Stack *s);

/**
 * \brief Is Stack empty?
 *
 * \param s instance of Stack
 *
 * \return 1 if stack empty, 0 otherwise
 */
uint8_t Stack_IsEmpty(Stack *s);

#endif /* STACK_H */

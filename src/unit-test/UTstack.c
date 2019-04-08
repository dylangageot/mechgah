#include "UTest.h"
#include "../common/stack.h"
#include <stdlib.h>


static int setup_Stack(void** state) {
	Stack *s = (Stack*) malloc(sizeof(Stack));
	if (s == NULL)
		return -1;
	*state = (void*) s;
    return 0;
}

static int teardown_Stack(void** state) {
	free(*state);
    return 0;
}

static void test_Stack_Init(void **state) {
	Stack *s = (Stack*) *state;
	s->index = 0xAA;
	Stack_Init(s);
	assert_int_equal(s->index, -1);
}

static void test_Stack_Push(void **state) {
	Stack *s = (Stack*) *state;
	int i;
	s->index = 0xAA;
	Stack_Init(s);
	assert_int_equal(Stack_Push(s, (void*)0xAABBCCDD), EXIT_SUCCESS);
	assert_int_equal(s->index, 0);
	assert_ptr_equal(s->data[s->index], (void*)0xAABBCCDD);
	for (i = 1; i < MAX_STACK; i++)
		assert_int_equal(Stack_Push(s, (void*) 0xAABBCCDD), EXIT_SUCCESS);
	assert_int_equal(Stack_Push(s, (void*) 0xAABBCCDD), EXIT_FAILURE);
		
}

static void test_Stack_Pop_IsEmpty(void **state) {
	Stack *s = (Stack*) *state;
	void *data;
	Stack_Init(s);
	assert_int_equal(Stack_IsEmpty(s), 1);
	Stack_Push(s, (void*)0xAABBCCDD);
	assert_int_equal(Stack_IsEmpty(s), 0);
	data = Stack_Pop(s);
	assert_ptr_equal(data, (void*) 0xAABBCCDD);
	assert_int_equal(s->index, -1);
	assert_int_equal(Stack_IsEmpty(s), 1);
	data = Stack_Pop(s);
	assert_ptr_equal(data, NULL);
}

int run_UTstack(void) {
	const struct CMUnitTest test_Stack[] = {
		cmocka_unit_test(test_Stack_Init),
		cmocka_unit_test(test_Stack_Push),
		cmocka_unit_test(test_Stack_Pop_IsEmpty),
	};
	int out = 0;
	out += cmocka_run_group_tests(test_Stack, setup_Stack, teardown_Stack);
	return out;
}

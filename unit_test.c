/*	CMocka library */
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "src/nes/mapper/nrom.h"

static int setup_zero(void **state) {
	*state = (void *) MapNROM_Create(NROM_32KIB, NROM_HORIZONTAL);
	if (*state == NULL)
		return -1;
	return 0;
}

static void test_increment(void **state) {
	assert_int_equal(1, 1); 
}

static int teardown(void **state) {
	if (*state != NULL) {
		MapNROM_Destroy(*state);
		return 0;
	} else
		return -1;
}

const struct CMUnitTest test_nrom[] = {
	cmocka_unit_test(test_increment),
};

int main() {
	return cmocka_run_group_tests(test_nrom, setup_zero, teardown);
}


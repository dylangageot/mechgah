#include "UTest.h"
#include "../nes/nes.h"
#include <stdlib.h>

static int setup_NES(void** state) {
    
	/* create a CPU instance */
    *state = (void *) NES_Create("src/unit-test/roms/nestest.nes");
    if (*state == NULL)
		return -1;

    return 0;
}

/* Execute Unit Test */
static void test_NES_Execution(void **state) {
	NES *self = (NES*) *state;
	assert_int_equal(NES_NextFrame(self, 0), EXIT_SUCCESS);
	assert_ptr_equal((void*) NES_Render(NULL), (void*) NULL); 
	assert_ptr_equal((void*) NES_Render(self), (void*) self->ppu->image); 
}


static int teardown_NES(void **state) {
	if (*state != NULL) {
		NES_Destroy((NES*) *state);
		return 0;
	}
    else {
		return -1;
    }
}

int run_UTnes(void) {
    const struct CMUnitTest test_NES[] = {
        cmocka_unit_test(test_NES_Execution),
    };
    int out = 0;
    out += cmocka_run_group_tests(test_NES, setup_NES, teardown_NES);
    return out;
}

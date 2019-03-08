#include "UTest.h"
#include "../nes/cpu/cpu.h"
#include "../nes/mapper/nrom.h"
#include <stdlib.h>

static int setup_CPU(void** state) {
    /* create a NROM Mapper*/
    Mapper* mapper =  (Mapper*)malloc(sizeof(Mapper));
    if (mapper == NULL) {
        return -1;
    }

    mapper->memoryMap = MapNROM_Create(NROM_16KIB, NROM_VERTICAL);
	mapper->destroyer = MapNROM_Destroy;
	mapper->ack = MapNROM_Ack;
	mapper->get = MapNROM_Get;

    /* create a CPU instance */

    *state = (void *)CPU_Create(mapper);
    if (*state == NULL)
		return -1;

    return 0;
}

static void test_PC_on_stack(void** state) {
    CPU* self = (CPU*) *state;

    /* setup values for test */
    uint8_t context = 0x01;
    self->PC = 0xFEDC;
    self->SP = 0xA3;

    /* execute function */
    CPU_InterruptManager(self, &context);

    /* get PCL from stack */
    uint8_t* ptr = (self->rmap->get)(self->rmap->memoryMap, AS_CPU, 0x01A2);
    uint16_t on_stack_PC = (uint16_t)(*ptr);

    /* get PCH from stack */
    ptr = (self->rmap->get)(self->rmap->memoryMap, AS_CPU, 0x01A3);
    on_stack_PC |= (uint16_t)(*ptr) << 8;

    assert_int_equal(on_stack_PC, 0xFEDC);
}

static void test_P_on_stack(void** state) {
    CPU* self = (CPU*) *state;

    /* setup values for test */
    uint8_t context = 0x01;
    self->P = 0x97;
    self->SP = 0xA3;

    /* execute function */
    CPU_InterruptManager(self, &context);

    /* get P from stack */
    uint8_t* ptr = (self->rmap->get)(self->rmap->memoryMap, AS_CPU, 0x01A1);
    uint8_t on_stack_P = *ptr;

    assert_int_equal(on_stack_P, 0x97);
}

static void test_PC_fetch(void** state) {
    CPU* self = (CPU*) *state;

    /* setup values for test */
    uint8_t context = 0x01;
    self->PC = 0xFEDC;
    self->SP = 0xA3;

    /* set next PC LSByte */
    uint8_t* ptr = (self->rmap->get)(self->rmap->memoryMap, AS_CPU, 0xFFFA);
    *ptr = 0xC9;

    /* set next PC MSByte */
    ptr = (self->rmap->get)(self->rmap->memoryMap, AS_CPU, 0xFFFB);
    *ptr = 0x5D;

    /* execute function */
    CPU_InterruptManager(self, &context);

    assert_int_equal(self->PC, 0x5DC9);
}

static void test_SP_decrement(void** state) {
    CPU* self = (CPU*) *state;

    /* setup values for test */
    uint8_t context = 0x01;
    self->SP = 0xA3;

    /* execute function */
    CPU_InterruptManager(self, &context);

    assert_int_equal(self->SP, 0xA0);

}

static void test_I_set(void** state) {
    CPU* self = (CPU*) *state;

    /* setup values for test */
    uint8_t context = 0x01;
    self->P &= ~(0x01 << 2);

    /* execute function */
    CPU_InterruptManager(self, &context);

    assert_int_equal((self->P >> 2) & 0x01, 1);

}

static void test_N_clear(void** state) {
    CPU* self = (CPU*) *state;

    /* setup value for test */
    uint8_t context = 0x01;

    /* execute function */
    CPU_InterruptManager(self, &context);

    assert_int_equal(context & 0x01, 0);
}

static void test_cycleCount(void** state) {
    CPU* self = (CPU*) *state;

    /* setup value for test */
    uint8_t context = 0x01;

    /* execute function */
    uint8_t cycleCount = CPU_InterruptManager(self, &context);

    assert_int_equal(cycleCount, 7);
}

static void test_null_CPU(void** state) {
    CPU* self = NULL;

    /* setup value for test */
    uint8_t context = 0x01;

    /* execute function */
    uint8_t cycleCount = CPU_InterruptManager(self, &context);

    assert_int_equal(cycleCount, 0);
}

static int teardown_CPU(void **state) {
	if (*state != NULL) {

		CPU *self = (CPU*) *state;
		self->rmap->destroyer(self->rmap->memoryMap);
		free((void*) self->rmap);

        CPU_Destroy(self);

		return 0;
	} else
		return -1;
}

int run_UTinterrupt(void) {
    const struct CMUnitTest test_interrupt[] = {
        cmocka_unit_test(test_PC_on_stack),
        cmocka_unit_test(test_P_on_stack),
        cmocka_unit_test(test_PC_fetch),
        cmocka_unit_test(test_SP_decrement),
        cmocka_unit_test(test_I_set),
        cmocka_unit_test(test_N_clear),
        cmocka_unit_test(test_cycleCount),
        cmocka_unit_test(test_null_CPU),
    };
    int out = 0;
    out += cmocka_run_group_tests(test_interrupt, setup_CPU, teardown_CPU);
    return out;
}

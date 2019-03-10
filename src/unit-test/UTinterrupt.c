#include "UTest.h"
#include "../nes/cpu/cpu.h"
#include "../nes/mapper/nrom.h"
#include <stdlib.h>

static int setup_CPU(void** state) {
    /* create a NROM Mapper*/
    Mapper* mapper =  (Mapper*)malloc(sizeof(Mapper));
    if (mapper == NULL)
        return -1;

    mapper->memoryMap = MapNROM_Create(NROM_16KIB, NROM_VERTICAL);
	mapper->destroyer = MapNROM_Destroy;
	mapper->ack = MapNROM_Ack;
	mapper->get = MapNROM_Get;

    /* create a CPU instance */

    *state = (void *)CPU_Create(mapper);
    if (*state == NULL)
		return -1;

    /* initialize the CPU */
    if (CPU_Init(*state))
        return -1;

    return 0;
}

/* Interrupts Unit Tests */

static void test_NMI(void** state) {
    CPU* self = (CPU*) *state;

    /* setup values for test */
    uint8_t context = 0x01;
    self->PC = 0xFEDC;
    self->P = 0xFB; /* NV_B DIZC */
    self->SP = 0xA3;

    /* set next PC LSByte */
    uint8_t* ptr = (self->rmap->get)(self->rmap->memoryMap, AS_CPU, NMI_JMP_ADD);
    *ptr = 0xC9;

    /* set next PC MSByte */
    ptr = (self->rmap->get)(self->rmap->memoryMap, AS_CPU, NMI_JMP_ADD+1);
    *ptr = 0x5D;

    /* execute function */
    uint8_t cycleCount = CPU_InterruptManager(self, &context);

    /* get PCL from stack */
    ptr = (self->rmap->get)(self->rmap->memoryMap, AS_CPU, 0x01A2);
    uint16_t on_stack_PC = (uint16_t)(*ptr);

    /* get PCH from stack */
    ptr = (self->rmap->get)(self->rmap->memoryMap, AS_CPU, 0x01A3);
    on_stack_PC |= (uint16_t)(*ptr) << 8;

    /* get P from stack */
    ptr = (self->rmap->get)(self->rmap->memoryMap, AS_CPU, 0x01A1);
    uint8_t on_stack_P = *ptr;

    /* PC on stack */
    assert_int_equal(on_stack_PC, 0xFEDC);
    /* PC fetch */
    assert_int_equal(self->PC, 0x5DC9);
    /* P on stack */
    assert_int_equal(on_stack_P, 0xEB); /* on stack, B is clear */
    /* set I flag */
    assert_int_equal(self->P, 0xFF);
    /* decrement SP */
    assert_int_equal(self->SP, 0xA0);
    /* cycleCount set to 7 */
    assert_int_equal(cycleCount, 7);
    /* clear N from context */
    assert_int_equal(context, 0);
}

static void test_INTERRUPT_STACK_PC(void** state) {
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

static void test_INTERRUPT_STACK_P(void** state) {
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

    assert_int_equal(on_stack_P, 0x87);
}

static void test_INTERRUPT_DECREMENT_SP(void** state) {
    CPU* self = (CPU*) *state;

    /* setup values for test */
    uint8_t context = 0x01;
    self->SP = 0xA3;

    /* execute function */
    CPU_InterruptManager(self, &context);

    assert_int_equal(self->SP, 0xA0);

}

static void test_INTERRUPT_SET_I(void** state) {
    CPU* self = (CPU*) *state;

    /* setup values for test */
    uint8_t context = 0x01;
    self->P &= ~(0x01 << 2);

    /* execute function */
    CPU_InterruptManager(self, &context);

    assert_int_equal((self->P >> 2) & 0x01, 1);

}

static void test_INETRRUPT_CYCLE_COUNT(void** state) {
    CPU* self = (CPU*) *state;

    /* setup value for test */
    uint8_t context = 0x01;

    /* execute function */
    uint8_t cycleCount = CPU_InterruptManager(self, &context);

    assert_int_equal(cycleCount, 7);
}

static void test_INTERRUPT_NULL_CPU(void** state) {
    CPU* self = NULL;

    /* setup value for test */
    uint8_t context = 0x01;

    /* execute function */
    uint8_t cycleCount = CPU_InterruptManager(self, &context);

    assert_int_equal(cycleCount, 0);
}

/* NMI only Unit Tests */
static void test_NMI_FETCH_PC(void** state) {
    CPU* self = (CPU*) *state;

    /* setup values for test */
    uint8_t context = 0x01;
    self->PC = 0xFEDC;
    self->SP = 0xA3;

    /* set next PC LSByte */
    uint8_t* ptr = (self->rmap->get)(self->rmap->memoryMap, AS_CPU, NMI_JMP_ADD);
    *ptr = 0xC9;

    /* set next PC MSByte */
    ptr = (self->rmap->get)(self->rmap->memoryMap, AS_CPU, NMI_JMP_ADD+1);
    *ptr = 0x5D;

    /* execute function */
    CPU_InterruptManager(self, &context);

    assert_int_equal(self->PC, 0x5DC9);
}

static void test_NMI_CLEAR_N(void** state) {
    CPU* self = (CPU*) *state;

    /* setup value for test */
    uint8_t context = 0x01;

    /* execute function */
    CPU_InterruptManager(self, &context);

    assert_int_equal(context, 0);
}

/* IRQ only Unit Tests with I flag set */

static void test_IRQ_PC_I_FLAG_SET(void** state) {
    CPU* self = (CPU*) *state;

    /* setup values for test */
    uint8_t context = 0x02;
    self->PC = 0xFEDC;
    self->SP = 0xA3;
    self->P = (0x01 << 2);

    /* set next PC LSByte */
    uint8_t* ptr = (self->rmap->get)(self->rmap->memoryMap, AS_CPU, IRQ_JMP_ADD);
    *ptr = 0x5F;

    /* set next PC MSByte */
    ptr = (self->rmap->get)(self->rmap->memoryMap, AS_CPU, IRQ_JMP_ADD+1);
    *ptr = 0x8E;

    /* execute function */
    CPU_InterruptManager(self, &context);

    assert_int_equal(self->PC, 0xFEDC);
}

static void test_IRQ_SP_I_FLAG_SET(void** state) {
    CPU* self = (CPU*) *state;

    /* setup values for test */
    uint8_t context = 0x02;
    self->PC = 0xFEDC;
    self->SP = 0xA3;
    self->P = (0x01 << 2);

    /* execute function */
    CPU_InterruptManager(self, &context);

    assert_int_equal(self->SP, 0xA3);
}

static void test_IRQ_CONTEXT_I_FLAG_SET(void** state) {
    CPU* self = (CPU*) *state;

    /* setup value for test */
    uint8_t context = 0x02;
    self->P = (0x01 << 2);

    /* execute function */
    CPU_InterruptManager(self, &context);

    assert_int_equal(context, 0x02);
}


/* IRQ only Unit Tests with I flag clear */

static void test_IRQ_FETCH_PC_I_FLAG_CLEAR(void** state) {
    CPU* self = (CPU*) *state;

    /* setup values for test */
    uint8_t context = 0x02;
    self->PC = 0xFEDC;
    self->SP = 0xA3;
    self->P = ~(0x01 << 2);

    /* set next PC LSByte */
    uint8_t* ptr = (self->rmap->get)(self->rmap->memoryMap, AS_CPU, IRQ_JMP_ADD);
    *ptr = 0x5F;

    /* set next PC MSByte */
    ptr = (self->rmap->get)(self->rmap->memoryMap, AS_CPU, IRQ_JMP_ADD+1);
    *ptr = 0x8E;

    /* execute function */
    CPU_InterruptManager(self, &context);

    assert_int_equal(self->PC, 0x8E5F);
}

static void test_IRQ_CLEAR_I_I_FLAG_CLEAR(void** state) {
    CPU* self = (CPU*) *state;

    /* setup value for test */
    uint8_t context = 0x02;
    self->P = ~(0x01 << 2);

    /* execute function */
    CPU_InterruptManager(self, &context);

    assert_int_equal(context, 0);
}

/* Conflict between NMI and IRQ */

static void test_INTERRUPT_CONFLICT_FETCH_PC(void** state) {
    CPU* self = (CPU*) *state;

    /* setup values for test */
    uint8_t context = 0x03;
    self->PC = 0xFEDC;
    self->SP = 0xA3;

    /* set next PC LSByte */
    uint8_t* ptr = (self->rmap->get)(self->rmap->memoryMap, AS_CPU, NMI_JMP_ADD);
    *ptr = 0x32;

    /* set next PC MSByte */
    ptr = (self->rmap->get)(self->rmap->memoryMap, AS_CPU, NMI_JMP_ADD+1);
    *ptr = 0xBE;

    ptr = (self->rmap->get)(self->rmap->memoryMap, AS_CPU, IRQ_JMP_ADD);
    *ptr = 0xFF;

    ptr = (self->rmap->get)(self->rmap->memoryMap, AS_CPU, IRQ_JMP_ADD+1);
    *ptr = 0xFF;

    /* execute function */
    CPU_InterruptManager(self, &context);

    assert_int_equal(self->PC, 0xBE32);
}

static void test_INTERRUPT_CONFLICT_CLEAR_N(void** state) {
    CPU* self = (CPU*) *state;

    /* setup value for test */
    uint8_t context = 0xFF;

    /* execute function */
    CPU_InterruptManager(self, &context);

    assert_int_equal(context, 0xFE);
}

/* No interrupt */

static void test_NO_INTERRUPT_CONTEXT(void** state) {
    CPU* self = (CPU*) *state;

    /* setup value for test */
    uint8_t context = 0;

    /* execute function */
    CPU_InterruptManager(self, &context);

    assert_int_equal(context, 0);
}

static void test_NO_INTERRUPT_PC(void** state) {
    CPU* self = (CPU*) *state;

    /* setup values for test */
    uint8_t context = 0;
    self->PC = 0xFEDC;

    /* execute function */
    CPU_InterruptManager(self, &context);

    assert_int_equal(self->PC, 0xFEDC);
}

static void test_NO_INTERRUPT_SP(void** state) {
    CPU* self = (CPU*) *state;

    /* setup values for test */
    uint8_t context = 0;
    self->SP = 0xA3;

    /* execute function */
    CPU_InterruptManager(self, &context);

    assert_int_equal(self->SP, 0xA3);
}
/* IRQ disable */

static int teardown_CPU(void **state) {
	if (*state != NULL) {

		CPU *self = (CPU*) *state;
		self->rmap->destroyer(self->rmap->memoryMap);
		free((void*) self->rmap);

        CPU_Destroy(self);

		return 0;
	}
    else {
		return -1;
    }
}

int run_UTinterrupt(void) {
    const struct CMUnitTest test_interrupt[] = {
        cmocka_unit_test(test_NMI),
        cmocka_unit_test(test_INTERRUPT_STACK_PC),
        cmocka_unit_test(test_INTERRUPT_STACK_P),
        cmocka_unit_test(test_INTERRUPT_DECREMENT_SP),
        cmocka_unit_test(test_INTERRUPT_SET_I),
        cmocka_unit_test(test_INETRRUPT_CYCLE_COUNT),
        cmocka_unit_test(test_INTERRUPT_NULL_CPU),
        cmocka_unit_test(test_NMI_FETCH_PC),
        cmocka_unit_test(test_NMI_CLEAR_N),
        cmocka_unit_test(test_IRQ_FETCH_PC_I_FLAG_CLEAR),
        cmocka_unit_test(test_IRQ_CLEAR_I_I_FLAG_CLEAR),
        cmocka_unit_test(test_IRQ_PC_I_FLAG_SET),
        cmocka_unit_test(test_IRQ_SP_I_FLAG_SET),
        cmocka_unit_test(test_IRQ_CONTEXT_I_FLAG_SET),
        cmocka_unit_test(test_INTERRUPT_CONFLICT_FETCH_PC),
        cmocka_unit_test(test_INTERRUPT_CONFLICT_CLEAR_N),
        cmocka_unit_test(test_NO_INTERRUPT_CONTEXT),
        cmocka_unit_test(test_NO_INTERRUPT_PC),
        cmocka_unit_test(test_NO_INTERRUPT_SP)
    };
    int out = 0;
    out += cmocka_run_group_tests(test_interrupt, setup_CPU, teardown_CPU);
    return out;
}

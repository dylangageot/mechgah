#include "UTest.h"
#include "../nes/cpu/cpu.h"
#include "../nes/mapper/nrom.h"
#include <stdlib.h>

static int setup_CPU(void** state) {
    /* create a NROM Mapper*/
    Mapper* mapper =  (Mapper*)malloc(sizeof(Mapper));
    if (mapper == NULL)
        return -1;

    Header config;
	config.mirroring = NROM_HORIZONTAL;
	config.romSize = NROM_16KIB;
	mapper->memoryMap = MapNROM_Create(&config);
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

static void test_RESET(void** state){
    CPU* self = (CPU*) *state;

    /* setup values for test */
    uint8_t context = 0x01;
    self->PC = 0xFEDC;
    /* NV_B DIZC */
    /* 1111 1011*/
    self->P = 0xFB;
    self->SP = 0xA3;

    /* set next PC LSByte */
    uint8_t* ptr = (self->rmap->get)(self->rmap->memoryMap, AS_CPU, RES_JMP_ADD);
    *ptr = 0xC9;

    /* set next PC MSByte */
    ptr = (self->rmap->get)(self->rmap->memoryMap, AS_CPU, RES_JMP_ADD+1);
    *ptr = 0x5D;

    /* execute function */
    uint8_t cycleCount = CPU_InterruptManager(self, &context);

    /* PC fetch */
    assert_int_equal(self->PC, 0x5DC9);
    /* set I flag */
    assert_int_equal(self->P, 0xFF);
    /* decrement SP */
    assert_int_equal(self->SP, 0xA0);
    /* cycleCount set to 7 */
    assert_int_equal(cycleCount, 7);
    /* clear R from context */
    assert_int_equal(context, 0);
}

static void test_NMI(void** state) {
    CPU* self = (CPU*) *state;

    /* setup values for test */
    uint8_t context = 0x02;
    self->PC = 0xFEDC;
    /* NV_B DIZC */
    /* 1111 1011*/
    self->P = 0xFB;
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

static void test_IRQ_I_FLAG_CLEAR(void** state) {
    CPU* self = (CPU*) *state;

    /* setup values for test */
    uint8_t context = 0x04;
    self->PC = 0xFEDC;
    /* NV_B DIZC */
    /* 1111 1011*/
    self->P = 0xFB;
    self->SP = 0xA3;

    /* set next PC LSByte */
    uint8_t* ptr = (self->rmap->get)(self->rmap->memoryMap, AS_CPU, IRQ_JMP_ADD);
    *ptr = 0xC9;

    /* set next PC MSByte */
    ptr = (self->rmap->get)(self->rmap->memoryMap, AS_CPU, IRQ_JMP_ADD+1);
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

static void test_IRQ_I_FLAG_SET(void** state) {
    CPU* self = (CPU*) *state;

    /* setup values for test */
    uint8_t context = 0x04;
    self->PC = 0xFEDC;
    /* NV_B DIZC */
    /* 1111 1111*/
    self->P = 0xFF;
    self->SP = 0xA3;

    /* execute function */
    uint8_t cycleCount = CPU_InterruptManager(self, &context);

    /* PC unchanged */
    assert_int_equal(self->PC, 0xFEDC);
    /* P unchanged */
    assert_int_equal(self->P, 0xFF);
    /* SP unchanged */
    assert_int_equal(self->SP, 0xA3);
    /* cycleCount set to O */
    assert_int_equal(cycleCount, 0);
    /* context unchanged */
    assert_int_equal(context, 0x04);
}

/* Conflict between NMI and IRQ */

static void test_NMI_IRQ_CONFLICT(void** state) {
    CPU* self = (CPU*) *state;

    /* setup values for test */
    uint8_t context = 0x06;
    self->PC = 0xFEDC;
    /* NV_B DIZC */
    /* 1111 1011*/
    self->P = 0xFB;
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
    assert_int_equal(context, 0x04);
}

/* No interrupt */

static void test_NO_INTERRUPT(void** state) {
    CPU* self = (CPU*) *state;

    /* setup values for test */
    uint8_t context = 0;
    self->PC = 0xFEDC;
    /* NV_B DIZC */
    /* 1111 1111*/
    self->P = 0xFF;
    self->SP = 0xA3;

    /* execute function */
    uint8_t cycleCount = CPU_InterruptManager(self, &context);

    /* PC unchanged */
    assert_int_equal(self->PC, 0xFEDC);
    /* P unchanged */
    assert_int_equal(self->P, 0xFF);
    /* SP unchanged */
    assert_int_equal(self->SP, 0xA3);
    /* cycleCount set to O */
    assert_int_equal(cycleCount, 0);
    /* context unchanged */
    assert_int_equal(context, 0);
}

/* Execute Unit Test */
static void test_CPU_Execute(void **state) {
	CPU *self = (CPU*) *state;
	Mapper *mapper = self->rmap;
	uint8_t *memory = mapper->get(mapper->memoryMap, AS_CPU, 0x8000);
	uint8_t context = 0;
	uint32_t clk = 0;
	
	/* Init register */
	self->PC = 0x8000;
	self->A = 1;
	self->P = 0;

	/* ADC $1AAA */
	memory[0] = 0x6D;
	memory[1] = 0xAA;
	memory[2] = 0x1A;
	/* JMP $1BAA */
	memory[3] = 0x4C;
	memory[4] = 0x00;
	memory[5] = 0x90;
	/* Set $1AAA = 0xAA */
	memory = mapper->get(mapper->memoryMap, AS_CPU, 0x1AAA);
	memory[0] = 0xAA;

	/* Expect 4 clock cycle to be used for ADC_ABS */
	assert_int_equal(CPU_Execute(self, &context, &clk), 4);
	/* Verify instruction execution */
	assert_int_equal(self->PC, 0x8003);
	assert_int_equal(self->A, 0xAB);
	/* Expect 7 clock cycle to be used after JMP_ABS */
	assert_int_equal(CPU_Execute(self, &context, &clk), 7);
	assert_int_equal(self->PC, 0x9000);
	assert_int_equal(self->A, 0xAB);
}


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

int run_UTcpu(void) {
    const struct CMUnitTest test_interrupt[] = {
        cmocka_unit_test(test_RESET),
        cmocka_unit_test(test_NMI),
        cmocka_unit_test(test_IRQ_I_FLAG_CLEAR),
        cmocka_unit_test(test_IRQ_I_FLAG_SET),
        cmocka_unit_test(test_NMI_IRQ_CONFLICT),
        cmocka_unit_test(test_NO_INTERRUPT)
    };
	const struct CMUnitTest test_cpu[] = {
		cmocka_unit_test(test_CPU_Execute),
	};
    int out = 0;
    out += cmocka_run_group_tests(test_interrupt, setup_CPU, teardown_CPU);
    out += cmocka_run_group_tests(test_cpu, setup_CPU, teardown_CPU);
    return out;
}

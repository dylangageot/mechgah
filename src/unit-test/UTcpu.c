#include "UTest.h"
#include "../nes/cpu/cpu.h"
#include "../nes/mapper/nrom.h"
#include <stdlib.h>

static int setup_CPU(void** state) {
    /* create a NROM Mapper*/
    Header config;
	config.mirroring = NROM_HORIZONTAL;
	config.romSize = NROM_16KIB;
	Mapper *mapper = MapNROM_Create(&config);

    /* create a CPU instance */
    *state = (void *)CPU_Create(mapper);
    if (*state == NULL)
		return -1;

    /* initialize the CPU */
    if (CPU_Init(*state))
        return -1;

    return 0;
}

static int setup_CPU_ultimate(void **state) {
	/* Load data from .nes */
	Mapper *mapper = loadROM("src/unit-test/roms/nestest.nes");
	if (mapper == NULL)
		return -1;
	/* Create instance of CPU */
	CPU *cpu = CPU_Create(mapper);
	if (cpu == NULL) {
		Mapper_Destroy(mapper);
		return -1;
	}
	/* Init CPU */
	CPU_Init(cpu);
	*state = (void*) cpu;
	return 0;
}

static int teardown_CPU_ultimate(void **state) {
	CPU *cpu = (CPU*) *state;
	if (cpu->mapper != NULL) {
		/* Free mapper data */
		Mapper_Destroy(cpu->mapper);
	}
	CPU_Destroy(cpu);
	return 0;
}

static void test_CPU_ultimate(void **state) {
	CPU *self = (CPU*) *state;
	FILE *fCPU = NULL, *fGoal = NULL;
	char strCPU[512], strGoal[512];
	int i;
	uint32_t clockCount = 0;
	uint8_t context = 1;

	/* Replace reset vector 0xC000 to launch automate test */
	*(Mapper_Get(self->mapper, AS_CPU, 0xFFFC)) = 0x00;

	/* Execute CPU for 5003 instructions (instruction before illegal opcode) */
	for (i = 0; i < 5003; i++) {
		assert_int_equal(CPU_Execute(self, &context, &clockCount), EXIT_SUCCESS);
	}

	/* Diff between log files to ensure that CPU is working as expected */
	assert_ptr_not_equal(fCPU = fopen("cpu.log", "r"), NULL);
	assert_ptr_not_equal(fGoal = fopen("src/unit-test/roms/nestest.log", "r"), 
						 NULL);
	for (i = 0; i < 5003; i++) {
		fgets(strCPU, 512, fCPU);	
		fgets(strGoal, 512, fGoal);	
		assert_int_equal(strcmp(strCPU,strGoal), 0);
	}

	fclose(fCPU);
	fclose(fGoal);
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
    uint8_t* ptr = Mapper_Get(self->mapper, AS_CPU, RES_JMP_ADD);
    *ptr = 0xC9;

    /* set next PC MSByte */
    ptr = Mapper_Get(self->mapper, AS_CPU, RES_JMP_ADD+1);
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
    uint8_t* ptr = Mapper_Get(self->mapper, AS_CPU, NMI_JMP_ADD);
    *ptr = 0xC9;

    /* set next PC MSByte */
    ptr = Mapper_Get(self->mapper, AS_CPU, NMI_JMP_ADD+1);
    *ptr = 0x5D;

    /* execute function */
    uint8_t cycleCount = CPU_InterruptManager(self, &context);

    /* get PCL from stack */
    ptr = Mapper_Get(self->mapper, AS_CPU, 0x01A2);
    uint16_t on_stack_PC = (uint16_t)(*ptr);

    /* get PCH from stack */
    ptr = Mapper_Get(self->mapper, AS_CPU, 0x01A3);
    on_stack_PC |= (uint16_t)(*ptr) << 8;

    /* get P from stack */
    ptr = Mapper_Get(self->mapper, AS_CPU, 0x01A1);
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
    uint8_t* ptr = Mapper_Get(self->mapper, AS_CPU, IRQ_JMP_ADD);
    *ptr = 0xC9;

    /* set next PC MSByte */
    ptr = Mapper_Get(self->mapper, AS_CPU, IRQ_JMP_ADD+1);
    *ptr = 0x5D;

    /* execute function */
    uint8_t cycleCount = CPU_InterruptManager(self, &context);

    /* get PCL from stack */
    ptr = Mapper_Get(self->mapper, AS_CPU, 0x01A2);
    uint16_t on_stack_PC = (uint16_t)(*ptr);

    /* get PCH from stack */
    ptr = Mapper_Get(self->mapper, AS_CPU, 0x01A3);
    on_stack_PC |= (uint16_t)(*ptr) << 8;

    /* get P from stack */
    ptr = Mapper_Get(self->mapper, AS_CPU, 0x01A1);
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
    uint8_t* ptr = Mapper_Get(self->mapper, AS_CPU, NMI_JMP_ADD);
    *ptr = 0xC9;

    /* set next PC MSByte */
    ptr = Mapper_Get(self->mapper, AS_CPU, NMI_JMP_ADD+1);
    *ptr = 0x5D;

    /* execute function */
    uint8_t cycleCount = CPU_InterruptManager(self, &context);

    /* get PCL from stack */
    ptr = Mapper_Get(self->mapper, AS_CPU, 0x01A2);
    uint16_t on_stack_PC = (uint16_t)(*ptr);

    /* get PCH from stack */
    ptr = Mapper_Get(self->mapper, AS_CPU, 0x01A3);
    on_stack_PC |= (uint16_t)(*ptr) << 8;

    /* get P from stack */
    ptr = Mapper_Get(self->mapper, AS_CPU, 0x01A1);
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
	uint8_t *memory = Mapper_Get(self->mapper, AS_CPU, 0x8000);
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
	memory = Mapper_Get(self->mapper, AS_CPU, 0x1AAA);
	memory[0] = 0xAA;

	/* Expect 4 clock cycle to be used for ADC_ABS */
	assert_int_equal(CPU_Execute(self, &context, &clk), EXIT_SUCCESS);
	assert_int_equal(clk, 4);
	/* Verify instruction execution */
	assert_int_equal(self->PC, 0x8003);
	assert_int_equal(self->A, 0xAB);
	/* Expect 7 clock cycle to be used after JMP_ABS */
	assert_int_equal(CPU_Execute(self, &context, &clk), EXIT_SUCCESS);
	assert_int_equal(clk, 7);
	assert_int_equal(self->PC, 0x9000);
	assert_int_equal(self->A, 0xAB);
}


static int teardown_CPU(void **state) {
	if (*state != NULL) {

		CPU *self = (CPU*) *state;
		Mapper_Destroy(self->mapper);
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
	const struct CMUnitTest test_cpu_ultimate[] = {
		cmocka_unit_test(test_CPU_ultimate),
	};
    int out = 0;
    out += cmocka_run_group_tests(test_interrupt, setup_CPU, teardown_CPU);
    out += cmocka_run_group_tests(test_cpu, setup_CPU, teardown_CPU);
    out += cmocka_run_group_tests(test_cpu_ultimate, setup_CPU_ultimate, teardown_CPU_ultimate);
    return out;
}

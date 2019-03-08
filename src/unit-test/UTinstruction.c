#include "UTest.h"
#include "../nes/cpu/instructions.h"
#include "../nes/mapper/nrom.h"
#include <stdlib.h>

static int setup_CPU(void **state) {
	*state = malloc(sizeof(CPU));
	if (*state == NULL)
		return -1;
	/* Init CPU struct */
	CPU *self = (CPU*) *state;
	self->SP = 0;
	self->PC = 0;
	self->A = 0;
	self->P = 0;
	self->X = 0;
	self->Y = 0;
	self->rmap = malloc(sizeof(Mapper));
	if (self->rmap == NULL) {
		free(self);
		return -1;
	}
	/* Init mapper struct with NROM */
	self->rmap->memoryMap = MapNROM_Create(NROM_16KIB, NROM_VERTICAL);
	self->rmap->destroyer = MapNROM_Destroy;
	self->rmap->ack = MapNROM_Ack;
	self->rmap->get = MapNROM_Get;
	return 0;
}

static void test_SET_SIGN(void **state) {
	CPU *self = (CPU*) *state;
	uint8_t val = -128;
	_SET_SIGN(self, &val);
	assert_int_equal(self->P & 0x80, 0x80);
	val = 127;
	_SET_SIGN(self, &val);
	assert_int_equal(self->P & 0x80, 0x00);
}

static void test_SET_ZERO(void **state) {
	CPU *self = (CPU*) *state;
	uint8_t val = 0;
	_SET_ZERO(self, &val);
	assert_int_equal(self->P & 0x02, 0x02);
	val = 255;
	_SET_ZERO(self, &val);
	assert_int_equal(self->P & 0x02, 0x00);
}

static void test_SET_CARRY(void **state) {
	CPU *self = (CPU*) *state;
	_SET_CARRY(self, 1);
	assert_int_equal(self->P & 0x01, 0x01);
	_SET_CARRY(self, 0);
	assert_int_equal(self->P & 0x01, 0x00);
}

static void test_SET_OVERFLOW(void **state) {
	CPU *self = (CPU*) *state;
	_SET_OVERFLOW(self, 1);
	assert_int_equal(self->P & 0x40, 0x40);
	_SET_OVERFLOW(self, 0);
	assert_int_equal(self->P & 0x40, 0x00);
}

static void test_SET_INTERRUPT(void **state) {
	CPU *self = (CPU*) *state;
	assert_int_equal(self->P & 0x04, 0x00);
	_SET_INTERRUPT(self);
	assert_int_equal(self->P & 0x04, 0x04);
}

static void test_SET_BREAK(void **state) {
	CPU *self = (CPU*) *state;
	assert_int_equal(self->P & 0x10, 0x00);
	_SET_BREAK(self);
	assert_int_equal(self->P & 0x10, 0x10);
}

static void test_REL_ADDR(void **state) {
	CPU *self = (CPU*) *state;
	self->PC = 0x8000;
	int8_t rel = -64;
	assert_int_equal(_REL_ADDR(self, &rel), 0x8000-64);
	rel = 64;
	assert_int_equal(_REL_ADDR(self, &rel), 0x8000+64);
}

static void test_SET_SR(void **state) {
	CPU *self = (CPU*) *state;
	uint8_t val = 0xAA;
	self->P = 0;
	_SET_SR(self, &val);
	assert_int_equal(self->P, val);
}

static void test_GET_SR(void **state) {
	CPU *self = (CPU*) *state;
	self->P = 0xAA;
	assert_int_equal(_GET_SR(self), 0xAA);
}

static void test_PULL(void **state) {
	CPU *self = (CPU*) *state;
	self->SP = 0xFE;
	uint8_t *ptr = self->rmap->get(self->rmap->memoryMap, AS_CPU, 0x0100 | 
															(self->SP + 1));
	*ptr = 0xAA;
	assert_int_equal(_PULL(self), 0xAA);
	assert_int_equal(self->SP, 0xFF);
}

static void test_PUSH(void **state) {
	CPU *self = (CPU*) *state;
	self->SP = 0xFF;
	uint8_t val = 0xAA;
	_PUSH(self, &val);
	assert_int_equal(self->SP, 0xFE);
	uint8_t *ptr = self->rmap->get(self->rmap->memoryMap, AS_CPU, 0x0100 | 
															(self->SP + 1));
	assert_int_equal(*ptr, 0xAA);
}

static void test_LOAD(void **state) {
	CPU *self = (CPU*) *state;
	uint8_t *ptr = self->rmap->get(self->rmap->memoryMap, AS_CPU, 0x1234); 
	*ptr = 0xAA;
	assert_int_equal(_LOAD(self, 0x1234), 0xAA); 
}

static void test_STORE(void **state) {
	CPU *self = (CPU*) *state;
	uint8_t val = 0xAA;
	_STORE(self, 0x1234, &val);
	uint8_t *ptr = self->rmap->get(self->rmap->memoryMap, AS_CPU, 0x1234); 
	assert_int_equal(*ptr, 0xAA); 
}

static void test_IF_CARRY(void **state) {
	CPU *self = (CPU*) *state;
	self->P = 0x01;
	assert_int_equal(_IF_CARRY(self), 1);
	self->P = 0x00;
	assert_int_equal(_IF_CARRY(self), 0);
}

static void test_IF_OVERFLOW(void **state) {
	CPU *self = (CPU*) *state;
	self->P = 0x70;
	assert_int_equal(_IF_OVERFLOW(self), 1);
	self->P = 0x00;
	assert_int_equal(_IF_OVERFLOW(self), 0);
}

static void test_IF_SIGN(void **state) {
	CPU *self = (CPU*) *state;
	self->P = 0x80;
	assert_int_equal(_IF_SIGN(self), 1);
	self->P = 0x00;
	assert_int_equal(_IF_SIGN(self), 0);

}

static void test_IF_ZERO(void **state) {
	CPU *self = (CPU*) *state;
	self->P = 0x02;
	assert_int_equal(_IF_ZERO(self), 1);
	self->P = 0x00;
	assert_int_equal(_IF_ZERO(self), 0);
}

static void test_IF_INTERRUPT(void **state) {
	CPU *self = (CPU*) *state;
	self->P = 0x04;
	assert_int_equal(_IF_INTERRUPT(self), 1);
	self->P = 0x00;
	assert_int_equal(_IF_INTERRUPT(self), 0);
}

static void test_IF_BREAK(void **state) {
	CPU *self = (CPU*) *state;
	self->P = 0x10;
	assert_int_equal(_IF_BREAK(self), 1);
	self->P = 0x00;
	assert_int_equal(_IF_BREAK(self), 0);
}

static void test_BRANCH(void **state) {
	CPU *self = (CPU*) *state;
	Instruction inst;
	uint8_t src = 0x40, clk = 0;
	inst.dataMem = &src;

	/* Test : Condition low */
	self->PC = 0x8000;
	clk = _BRANCH(self, &inst, 0);
	assert_int_equal(clk, 2);
	assert_int_equal(self->PC, 0x8000);
	/* Test : Condition high */
	self->PC = 0x8000;
	clk = _BRANCH(self, &inst, 1);
	assert_int_equal(clk, 3);
	assert_int_equal(self->PC, 0x8000 + src);
	/* Test : Carry bit low and branch on different page */
	self->PC = 0x8000;
	src = 0xC0;
	clk = _BRANCH(self, &inst, 1);
	assert_int_equal(clk, 4);
	assert_int_equal(self->PC, 0x7FC0);
}

static void test_ADC(void **state) {
	CPU *self = (CPU*) *state;
	Instruction inst;
	uint8_t src = 0xAA, clk = 0;
	inst.opcode.inst = _ADC;
	inst.dataMem = &src;

	/* Test ADC general behavior */
	/* Test : Sign bit */
	inst.opcode.addressingMode = IMM;
	self->P = 0;
	self->A = 0x11;
	clk = _ADC(self, &inst);
	assert_int_equal(clk, 2);
	assert_int_equal(self->A, 0xAA + 0x11);
	assert_int_equal(self->P, 0x80);
	/* Test : Overflow and Sign bit */
	self->P = 0;
	self->A = 0x7F;
	src = 1;
	clk = _ADC(self, &inst);
	assert_int_equal(clk, 2);
	assert_int_equal(self->A, 0x7F + 1);
	assert_int_equal(self->P, 0xC0);
	/* Test : Zero and Carry bit */
	self->P = 0;
	self->A = 0xFF;
	src = 1;
	clk = _ADC(self, &inst);
	assert_int_equal(clk, 2);
	assert_int_equal(self->A, 0x00);
	assert_int_equal(self->P, 0x03);
	/* Test : Add with carry */
	self->P = 0x01;
	self->A = 1;
	src = 1;
	clk = _ADC(self, &inst);
	assert_int_equal(clk, 2);
	assert_int_equal(self->A, 0x03);
	assert_int_equal(self->P, 0x00);
	
	/* Test addressing mode clock */
	inst.pageCrossed = 1;
	inst.opcode.addressingMode = ZER;
	clk = _ADC(self, &inst);
	assert_int_equal(clk, 3);
	inst.opcode.addressingMode = ZEX;
	clk = _ADC(self, &inst);
	assert_int_equal(clk, 4);
	inst.opcode.addressingMode = ABS;
	clk = _ADC(self, &inst);
	assert_int_equal(clk, 4);
	inst.opcode.addressingMode = ABX;
	clk = _ADC(self, &inst);
	assert_int_equal(clk, 5);
	inst.pageCrossed = 0;
	inst.opcode.addressingMode = ABX;
	clk = _ADC(self, &inst);
	assert_int_equal(clk, 4);
	inst.pageCrossed = 1;
	inst.opcode.addressingMode = ABY;
	clk = _ADC(self, &inst);
	assert_int_equal(clk, 5);
	inst.pageCrossed = 0;
	inst.opcode.addressingMode = ABY;
	clk = _ADC(self, &inst);
	assert_int_equal(clk, 4);
	inst.pageCrossed = 1;
	inst.opcode.addressingMode = INX;
	clk = _ADC(self, &inst);
	assert_int_equal(clk, 6);
	inst.opcode.addressingMode = INY;
	clk = _ADC(self, &inst);
	assert_int_equal(clk, 6);
	inst.pageCrossed = 0;
	inst.opcode.addressingMode = INY;
	clk = _ADC(self, &inst);
	assert_int_equal(clk, 5);
}

static void test_ASL(void **state) {
	CPU *self = (CPU*) *state;
	Instruction inst;
	uint8_t src = 0xAA, clk = 0;
	inst.dataMem = &src;

	/* Test ASL general behavior */
	/* Test : Carry bit */
	inst.opcode.addressingMode = ACC;
	self->P = 0;
	clk = _ASL(self, &inst);
	assert_int_equal(clk, 2);
	assert_int_equal(src, (0xAA << 1) & 0xFF);
	assert_int_equal(self->P, 0x01);
	/* Test : Zero and carry bit */
	self->P = 0;
	src = 0x80;
	clk = _ASL(self, &inst);
	assert_int_equal(clk, 2);
	assert_int_equal(src, 0x00);
	assert_int_equal(self->P, 0x03);
	/* Test : Sign and carry bit */
	self->P = 0;
	src = 0xC0;
	clk = _ASL(self, &inst);
	assert_int_equal(clk, 2);
	assert_int_equal(src, 0x80);
	assert_int_equal(self->P, 0x81);
	/* Test : No carry bit */
	self->P = 0;
	src = 0x3F;
	clk = _ASL(self, &inst);
	assert_int_equal(clk, 2);
	assert_int_equal(src, 0x3F << 1);
	assert_int_equal(self->P, 0x00);
	
	/* Test addressing mode clock */
	inst.pageCrossed = 1;
	inst.opcode.addressingMode = ACC;
	clk = _ASL(self, &inst);
	assert_int_equal(clk, 2);
	inst.opcode.addressingMode = ZER;
	clk = _ASL(self, &inst);
	assert_int_equal(clk, 5);
	inst.opcode.addressingMode = ZEX;
	clk = _ASL(self, &inst);
	assert_int_equal(clk, 6);
	inst.opcode.addressingMode = ABS;
	clk = _ASL(self, &inst);
	assert_int_equal(clk, 6);
	inst.opcode.addressingMode = ABX;
	clk = _ASL(self, &inst);
	assert_int_equal(clk, 7);
}

static void test_BCC(void **state) {
	CPU *self = (CPU*) *state;
	Instruction inst;
	uint8_t src = 0x40, clk = 0;
	inst.dataMem = &src;
	self->PC = 0x8000;

	/* Test BCC general behavior */
	/* Test : Carry bit high */
	self->P = 0x01;
	clk = _BCC(self, &inst);
	assert_int_equal(clk, 2);
	/* Test : Carry bit low */
	self->P = 0x00;
	clk = _BCC(self, &inst);
	assert_int_equal(clk, 3);
}

static void test_BCS(void **state) {
	CPU *self = (CPU*) *state;
	Instruction inst;
	uint8_t src = 0x40, clk = 0;
	inst.dataMem = &src;
	self->PC = 0x8000;

	/* Test BCS general behavior */
	/* Test : Carry bit low */
	self->P = 0x00;
	clk = _BCS(self, &inst);
	assert_int_equal(clk, 2);
	/* Test : Carry bit high */
	self->P = 0x01;
	clk = _BCS(self, &inst);
	assert_int_equal(clk, 3);
}

static void test_BEQ(void **state) {
	CPU *self = (CPU*) *state;
	Instruction inst;
	uint8_t src = 0x40, clk = 0;
	inst.dataMem = &src;
	self->PC = 0x8000;

	/* Test BEQ general behavior */
	/* Test : Zero bit low */
	self->P = 0x00;
	clk = _BEQ(self, &inst);
	assert_int_equal(clk, 2);
	/* Test : Zero bit high */
	self->P = 0x02;
	clk = _BEQ(self, &inst);
	assert_int_equal(clk, 3);
}

static void test_BMI(void **state) {
	CPU *self = (CPU*) *state;
	Instruction inst;
	uint8_t src = 0x40, clk = 0;
	inst.dataMem = &src;
	self->PC = 0x8000;

	/* Test BMI general behavior */
	/* Test : Sign bit low */
	self->P = 0x00;
	clk = _BMI(self, &inst);
	assert_int_equal(clk, 2);
	/* Test : Sign bit high */
	self->P = 0x80;
	clk = _BMI(self, &inst);
	assert_int_equal(clk, 3);
}

static void test_BNE(void **state) {
	CPU *self = (CPU*) *state;
	Instruction inst;
	uint8_t src = 0x40, clk = 0;
	inst.dataMem = &src;
	self->PC = 0x8000;

	/* Test BNE general behavior */
	/* Test : Zero bit high */
	self->P = 0x02;
	clk = _BNE(self, &inst);
	assert_int_equal(clk, 2);
	/* Test : Zero bit low */
	self->P = 0x00;
	clk = _BNE(self, &inst);
	assert_int_equal(clk, 3);
}

static void test_BPL(void **state) {
	CPU *self = (CPU*) *state;
	Instruction inst;
	uint8_t src = 0x40, clk = 0;
	inst.dataMem = &src;
	self->PC = 0x8000;

	/* Test BPL general behavior */
	/* Test : Sign bit high */
	self->P = 0x80;
	clk = _BPL(self, &inst);
	assert_int_equal(clk, 2);
	/* Test : Sign bit low */
	self->P = 0x00;
	clk = _BPL(self, &inst);
	assert_int_equal(clk, 3);
}

static void test_BVC(void **state) {
	CPU *self = (CPU*) *state;
	Instruction inst;
	uint8_t src = 0x40, clk = 0;
	inst.dataMem = &src;
	self->PC = 0x8000;

	/* Test BVC general behavior */
	/* Test : Overflow bit high */
	self->P = 0x40;
	clk = _BVC(self, &inst);
	assert_int_equal(clk, 2);
	/* Test : Overflow bit low */
	self->P = 0x00;
	clk = _BVC(self, &inst);
	assert_int_equal(clk, 3);
}

static void test_BVS(void **state) {
	CPU *self = (CPU*) *state;
	Instruction inst;
	uint8_t src = 0x40, clk = 0;
	inst.dataMem = &src;
	self->PC = 0x8000;

	/* Test BVS general behavior */
	/* Test : Overflow bit low */
	self->P = 0x00;
	clk = _BVS(self, &inst);
	assert_int_equal(clk, 2);
	/* Test : Overflow bit high */
	self->P = 0x40;
	clk = _BVS(self, &inst);
	assert_int_equal(clk, 3);
}

static int teardown_CPU(void **state) {
	if (*state != NULL) {
		CPU *self = (CPU*) *state;
		self->rmap->destroyer(self->rmap->memoryMap);
		free((void*) self->rmap);
		free((void*) self);
		return 0;
	} else
		return -1;
}

int run_instruction(void) {
	const struct CMUnitTest test_instruction_macro[] = {
		cmocka_unit_test(test_SET_SIGN),
		cmocka_unit_test(test_SET_ZERO),
		cmocka_unit_test(test_SET_CARRY),
		cmocka_unit_test(test_SET_OVERFLOW),
		cmocka_unit_test(test_SET_INTERRUPT),
		cmocka_unit_test(test_SET_BREAK),
		cmocka_unit_test(test_REL_ADDR),
		cmocka_unit_test(test_SET_SR),
		cmocka_unit_test(test_GET_SR),
		cmocka_unit_test(test_PULL),
		cmocka_unit_test(test_PUSH),
		cmocka_unit_test(test_LOAD),
		cmocka_unit_test(test_STORE),
		cmocka_unit_test(test_IF_CARRY),
		cmocka_unit_test(test_IF_OVERFLOW),
		cmocka_unit_test(test_IF_SIGN),
		cmocka_unit_test(test_IF_ZERO),
		cmocka_unit_test(test_IF_INTERRUPT),
		cmocka_unit_test(test_IF_BREAK),
		cmocka_unit_test(test_BRANCH),
	};
	const struct CMUnitTest test_instruction[] = {
		cmocka_unit_test(test_ADC),
		cmocka_unit_test(test_ASL),
		cmocka_unit_test(test_BCC),
		cmocka_unit_test(test_BCS),
		cmocka_unit_test(test_BEQ),
		cmocka_unit_test(test_BMI),
		cmocka_unit_test(test_BNE),
		cmocka_unit_test(test_BPL),
		cmocka_unit_test(test_BVC),
		cmocka_unit_test(test_BVS),
	};
	int out = 0;
	out += cmocka_run_group_tests(test_instruction_macro, setup_CPU, teardown_CPU);
	out += cmocka_run_group_tests(test_instruction, setup_CPU, teardown_CPU);
	return out;
}

#include "UTest.h"
#include "../nes/cpu/instructions.h"
#include "../nes/mapper/nrom.h"
#include <stdlib.h>
#include <stdio.h>

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
	Header config;
	config.mirroring = NROM_HORIZONTAL;
	config.romSize = NROM_16KIB;
	self->rmap->memoryMap = MapNROM_Create(&config);
	self->rmap->destroyer = MapNROM_Destroy;
	self->rmap->ack = MapNROM_Ack;
	self->rmap->get = MapNROM_Get;
	return 0;
}

static void test_instruction_fetch(void **state){
	CPU *self =(CPU*)*state;
	Mapper *mapper = self->rmap;
	Instruction * instru = malloc(sizeof(Instruction));
	if(instru == NULL) return;

	*(mapper->get(mapper->memoryMap, AS_CPU, 0x80F1)) = 0x12;
	*(mapper->get(mapper->memoryMap, AS_CPU, 0x80F2)) = 0x13;
	/* 0x98 -> opcode d'une instruction utilisant comme m_d IMPLED*/
	self->PC = 0x80F0;
	*(mapper->get(mapper->memoryMap, AS_CPU, 0x80F0)) = 0x98;
	assert_int_equal(Instruction_Fetch(instru,self),1);
	/* 0xB5 -> opcode d'une instruction utilisant comme m_d ZEX*/
	self->PC = 0x80F0;
	*(mapper->get(mapper->memoryMap, AS_CPU, 0x80F0)) = 0xB5;
	assert_int_equal(Instruction_Fetch(instru,self),1);
	assert_int_equal(instru->opcodeArg[0],0x12);
	/* 0x98 -> opcode d'une instruction utilisant comme m_d ABX*/
	self->PC = 0x80F0;
	*(mapper->get(mapper->memoryMap, AS_CPU, 0x80F0)) = 0xD9;
	assert_int_equal(Instruction_Fetch(instru,self),1);
	assert_int_equal(instru->opcodeArg[0],0x12);
	assert_int_equal(instru->opcodeArg[1],0x13);
	/* 0x03 -> opcode inexistant*/
	self->PC = 0x80F0;
	*(mapper->get(mapper->memoryMap, AS_CPU, 0x80F0)) = 0x03;
	assert_int_equal(Instruction_Fetch(NULL,NULL),0);
	assert_int_equal(Instruction_Fetch(instru,self),0);

	assert_int_equal(Instruction_Fetch(NULL,NULL),0);

	free(instru);
}

static void test_addressing_IMP(void **state){
	CPU *self =(CPU*)*state;
	Instruction * instru = malloc(sizeof(Instruction));
	if(instru == NULL) return;

	instru->opcode.addressingMode = IMP;

	assert_int_equal(Instruction_Resolve(instru,self),1);
	free(instru);
}

static void test_addressing_ACC(void **state){
	CPU *self =(CPU*)*state;
	uint8_t vTest = 0x55;
	Instruction * instru = malloc(sizeof(Instruction));
	if(instru == NULL) return;

	instru->opcode.addressingMode = ACC;
	instru->dataMem = NULL;
	self->A = vTest;

	Instruction_Resolve(instru,self);
	assert_int_equal(*(instru->dataMem),vTest);
	free(instru);
}

static void test_addressing_ZEX(void **state){
	CPU *self=(CPU*)*state;
	Mapper *mapper = self->rmap;
	uint8_t vTest = 0x25;
	Instruction * instru = malloc(sizeof(Instruction));
	if(instru == NULL) return;

	*(mapper->get(mapper->memoryMap, AS_CPU, 0x0013)) = vTest;

	instru->opcode.addressingMode = ZEX;
	instru->opcodeArg[0] = 0x0E;
	self->X = 0x05;
	instru->dataMem = NULL;


	Instruction_Resolve(instru,self);
	assert_int_equal(*(instru->dataMem),vTest);
	free(instru);
}

static void test_addressing_ZEY(void **state){
	CPU *self=(CPU*)*state;
	Mapper *mapper = self->rmap;
	uint8_t vTest = 25;
	Instruction * instru = malloc(sizeof(Instruction));
	if(instru == NULL) return;

	*(mapper->get(mapper->memoryMap, AS_CPU, 0x000A)) = vTest;

	instru->opcode.addressingMode = ZEY;
	instru->opcodeArg[0] = 0x08;
	self->Y = 0x02;
	instru->dataMem = NULL;

	Instruction_Resolve(instru,self);
	assert_int_equal(*(instru->dataMem),vTest);
	free(instru);
}

static void test_addressing_INX(void **state){
	CPU *self=(CPU*)*state;
	Mapper *mapper = self->rmap;
	uint8_t vTest = 0x25;
	Instruction * instru = malloc(sizeof(Instruction));
	if(instru == NULL) return;

	*(mapper->get(mapper->memoryMap, AS_CPU, 0x2415)) = vTest;
	*(mapper->get(mapper->memoryMap, AS_CPU, 0x0043)) = 0x15;
	*(mapper->get(mapper->memoryMap, AS_CPU, 0x0044)) = 0x24;

	instru->opcode.addressingMode = INX;
	instru->opcodeArg[0] = 0x3E;
	self->X = 0x05;
	instru->dataMem = NULL;


	Instruction_Resolve(instru,self);
	assert_int_equal(*(instru->dataMem),vTest);
	free(instru);
}

static void test_addressing_INY(void **state){
	CPU *self=(CPU*)*state;
	Mapper *mapper = self->rmap;
	uint8_t vTest = 0x6D;
	Instruction * instru = malloc(sizeof(Instruction));
	if(instru == NULL) return;

	*(mapper->get(mapper->memoryMap, AS_CPU, 0x2204)) = vTest;
	*(mapper->get(mapper->memoryMap, AS_CPU, 0x004C)) = 0x05;
	*(mapper->get(mapper->memoryMap, AS_CPU, 0x004D)) = 0x21;

	instru->opcode.addressingMode = INY;
	instru->opcodeArg[0] = 0x4C;
	self->Y = 0xFF;
	instru->dataMem = NULL;


	Instruction_Resolve(instru,self);
	assert_int_equal(*(instru->dataMem),vTest);
	assert_int_equal(instru->pageCrossed,1);
	free(instru);
}

static void test_addressing_IMM(void **state){
	CPU *self=(CPU*)*state;
	uint8_t vTest = 0x25;
	Instruction * instru = malloc(sizeof(Instruction));
	if(instru == NULL) return;

	instru->opcode.addressingMode = IMM;
	instru->opcodeArg[0] = vTest;

	Instruction_Resolve(instru,self);
	assert_int_equal(*(instru->dataMem),vTest);
	free(instru);
}

static void test_addressing_ZER(void **state){
	CPU *self=(CPU*)*state;
	Mapper *mapper = self->rmap;
	uint8_t vTest = 0xF4;
	Instruction * instru = malloc(sizeof(Instruction));
	if(instru == NULL) return;

	*(mapper->get(mapper->memoryMap, AS_CPU, 0x00F4)) = vTest;

	instru->opcode.addressingMode = ZER;
	instru->opcodeArg[0] = vTest;

	Instruction_Resolve(instru,self);
	assert_int_equal(*(instru->dataMem),vTest);
	free(instru);
}

static void test_addressing_REL(void **state){
	CPU *self=(CPU*)*state;
	uint8_t vTest = 0x25;
	Instruction * instru = malloc(sizeof(Instruction));
	if(instru == NULL) return;

	instru->opcode.addressingMode = REL;
	instru->opcodeArg[0] = vTest;

	Instruction_Resolve(instru,self);
	assert_int_equal(*(instru->dataMem),vTest);
	free(instru);
}

static void test_addressing_ABS(void **state){
	CPU *self=(CPU*)*state;
	Mapper *mapper = self->rmap;
	uint8_t vTest = 0xF4;
	Instruction * instru = malloc(sizeof(Instruction));
	if(instru == NULL) return;

	*(mapper->get(mapper->memoryMap, AS_CPU, 0x31F6)) = vTest;

	instru->opcode.addressingMode = ABS;
	instru->opcodeArg[0] = 0xF6;
	instru->opcodeArg[1] = 0x31;

	Instruction_Resolve(instru,self);
	assert_int_equal(*(instru->dataMem),vTest);
	free(instru);
}

static void test_addressing_ABX(void **state){
	CPU *self=(CPU*)*state;
	Mapper *mapper = self->rmap;
	uint8_t vTest = 0xF4;
	Instruction * instru = malloc(sizeof(Instruction));
	if(instru == NULL) return;

	*(mapper->get(mapper->memoryMap, AS_CPU, 0x32F5)) = vTest;

	instru->opcode.addressingMode = ABX;
	self->X = 0xFF;
	instru->opcodeArg[0] = 0xF6;
	instru->opcodeArg[1] = 0x31;

	Instruction_Resolve(instru,self);
	assert_int_equal(*(instru->dataMem),vTest);
	assert_int_equal(instru->pageCrossed,1);
	free(instru);
}

static void test_addressing_ABY(void **state){
	CPU *self=(CPU*)*state;
	Mapper *mapper = self->rmap;
	uint8_t vTest = 0xF6;
	Instruction * instru = malloc(sizeof(Instruction));
	if(instru == NULL) return;

	*(mapper->get(mapper->memoryMap, AS_CPU, 0x12A4)) = vTest;

	instru->opcode.addressingMode = ABY;
	self->Y = 0xFF;
	instru->opcodeArg[0] = 0xA5;
	instru->opcodeArg[1] = 0x11;

	Instruction_Resolve(instru,self);
	assert_int_equal(*(instru->dataMem),vTest);
	assert_int_equal(instru->pageCrossed,1);
	free(instru);
}

static void test_addressing_ABI(void **state){
	CPU *self=(CPU*)*state;
	Mapper *mapper = self->rmap;
	uint8_t vTest = 0x25;
	Instruction * instru = malloc(sizeof(Instruction));
	if(instru == NULL) return;

	*(mapper->get(mapper->memoryMap, AS_CPU, 0x215F)) = 0x76;
	*(mapper->get(mapper->memoryMap, AS_CPU, 0x2160)) = 0x30;
	*(mapper->get(mapper->memoryMap, AS_CPU, 0x3076)) = 0x25;

	instru->opcode.addressingMode = ABI;
	instru->opcodeArg[0] = 0x5F;
	instru->opcodeArg[1] = 0x21;

	Instruction_Resolve(instru,self);
	assert_int_equal(*(instru->dataMem),vTest);
	free(instru);
}

static void test_addressing_MIS(void **state){
	CPU *self=(CPU*)*state;
	Instruction * instru = malloc(sizeof(Instruction));
	if(instru == NULL) return;

	instru->opcode.addressingMode = NUL;

	Instruction_Resolve(instru,self);
	assert_int_equal(Instruction_Resolve(instru,self),0);
	assert_int_equal(Instruction_Resolve(NULL,NULL),0);
	free(instru);
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
	inst.opcode.cycle = 2;

	/* Test : Condition low */
	self->PC = 0x8000;
	clk = _BRANCH(self, &inst, 0);
	assert_int_equal(clk, 2);
	assert_int_equal(self->PC, 0x8000);
	/* Test : Condition high */
	inst.opcode.cycle = 2;
	self->PC = 0x8000;
	clk = _BRANCH(self, &inst, 1);
	assert_int_equal(clk, 3);
	assert_int_equal(self->PC, 0x8000 + src);
	/* Test : Carry bit low and branch on different page */
	inst.opcode.cycle = 2;
	self->PC = 0x8000;
	src = 0xC0;
	clk = _BRANCH(self, &inst, 1);
	assert_int_equal(clk, 4);
	assert_int_equal(self->PC, 0x7FC0);
}

static void test_ADC(void **state) {
	CPU *self = (CPU*) *state;
	Instruction inst;
	uint8_t (*ptr)(CPU*, Instruction*)  = _ADC;
	uint8_t src = 0xAA, clk = 0;
	inst.dataMem = &src;
	inst.pageCrossed = 0;

	/* Verify Opcode LUT */
	inst.opcode = Opcode_Get(0x69); /* ADC IMM */
	assert_ptr_equal(ptr, inst.opcode.inst);

	/* Test ADC general behavior */
	/* Test : Sign bit */
	self->P = 0;
	self->A = 0x11;
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 2);
	assert_int_equal(self->A, 0xAA + 0x11);
	assert_int_equal(self->P, 0x80);
	/* Test : Overflow and Sign bit */
	self->P = 0;
	self->A = 0x7F;
	src = 1;
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 2);
	assert_int_equal(self->A, 0x7F + 1);
	assert_int_equal(self->P, 0xC0);
	/* Test : Zero and Carry bit */
	self->P = 0;
	self->A = 0xFF;
	src = 1;
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 2);
	assert_int_equal(self->A, 0x00);
	assert_int_equal(self->P, 0x03);
	/* Test : Add with carry */
	self->P = 0x01;
	self->A = 1;
	src = 1;
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 2);
	assert_int_equal(self->A, 0x03);
	assert_int_equal(self->P, 0x00);

	/* Test addressing mode clock */
	inst.opcode = Opcode_Get(0x65); /* ADC ZER */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 3);

	inst.opcode = Opcode_Get(0x75); /* ADC ZEX */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 4);

	inst.opcode = Opcode_Get(0x6D); /* ADC ABS */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 4);

	inst.opcode = Opcode_Get(0x7D); /* ADC ABX */
	assert_ptr_equal(ptr, inst.opcode.inst);
	inst.pageCrossed = 1;
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 5);
	inst.pageCrossed = 0;

	inst.opcode = Opcode_Get(0x7D); /* ADC ABX */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 4);

	inst.pageCrossed = 1;
	inst.opcode = Opcode_Get(0x79); /* ADC ABY */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 5);

	inst.pageCrossed = 0;
	inst.opcode = Opcode_Get(0x79); /* ADC ABY */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 4);

	inst.opcode = Opcode_Get(0x61); /* ADC INX */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 6);

	inst.pageCrossed = 1;
	inst.opcode = Opcode_Get(0x71); /* ADC INY */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 6);

	inst.pageCrossed = 0;
	inst.opcode = Opcode_Get(0x71); /* ADC INY */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 5);
}

static void test_ASL(void **state) {
	CPU *self = (CPU*) *state;
	Instruction inst;
	uint8_t (*ptr)(CPU*, Instruction*) = _ASL;
	uint8_t src = 0xAA, clk = 0;
	inst.dataMem = &src;
	inst.pageCrossed = 1;

	/* Verify Opcode LUT */
	inst.opcode = Opcode_Get(0x0A); /* ASL ACC */
	assert_ptr_equal(ptr, inst.opcode.inst);

	/* Test ASL general behavior */
	/* Test : Carry bit */
	self->P = 0;
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 2);
	assert_int_equal(src, (0xAA << 1) & 0xFF);
	assert_int_equal(self->P, 0x01);
	/* Test : Zero and carry bit */
	self->P = 0;
	src = 0x80;
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 2);
	assert_int_equal(src, 0x00);
	assert_int_equal(self->P, 0x03);
	/* Test : Sign and carry bit */
	self->P = 0;
	src = 0xC0;
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 2);
	assert_int_equal(src, 0x80);
	assert_int_equal(self->P, 0x81);
	/* Test : No carry bit */
	self->P = 0;
	src = 0x3F;
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 2);
	assert_int_equal(src, 0x3F << 1);
	assert_int_equal(self->P, 0x00);

	/* Test addressing mode clock */
	inst.opcode = Opcode_Get(0x06); /* ASL ZER */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 5);

	inst.opcode = Opcode_Get(0x16); /* ASL ZEX */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 6);

	inst.opcode = Opcode_Get(0x0E); /* ASL ABS */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 6);

	inst.opcode = Opcode_Get(0x1E); /* ASL ABX */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 7);
}

static void test_BXX(void **state, uint8_t keep, uint8_t branch,
		uint8_t opcode, uint8_t (*ptr)(CPU*, Instruction*)) {
	CPU *self = (CPU*) *state;
	Instruction inst;
	uint8_t src = 0x40, clk = 0;
	inst.dataMem = &src;
	self->PC = 0x8000;

	/* Verify Opcode LUT */
	inst.opcode = Opcode_Get(opcode);
	assert_ptr_equal(ptr, inst.opcode.inst);

	/* Test BXX general behavior */
	/* Test : Conserve PC */
	self->P = keep;
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 2);
	/* Test : Branch */
	self->P = branch;
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 3);
}

static void test_BCC(void **state) {
	test_BXX(state, 0x01, 0x00, 0x90, _BCC);
}

static void test_BCS(void **state) {
	test_BXX(state, 0x00, 0x01, 0xB0, _BCS);
}

static void test_BEQ(void **state) {
	test_BXX(state, 0x00, 0x02, 0xF0, _BEQ);
}

static void test_BIT(void **state) {
	CPU *self = (CPU*) *state;
	Instruction inst;
	uint8_t (*ptr)(CPU*, Instruction*) = _BIT;
	uint8_t src = 0xC0, clk = 0;
	inst.dataMem = &src;
	inst.pageCrossed = 1; /* Supposed to have no effect */

	/* Verify Opcode LUT */
	inst.opcode = Opcode_Get(0x24); /* BIT ZER */
	assert_ptr_equal(ptr, inst.opcode.inst);

	/* Test BIT general behavior */
	/* Test : Sign, overflow and zero bit */
	self->A = 0;
	self->P = 0;
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 3);
	assert_int_equal(self->P, 0xC2);
	/* Test : Sign and overflow bit */
	self->A = 0xC0;
	self->P = 0;
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 3);
	assert_int_equal(self->P, 0xC0);
	/* Test : No bit */
	self->A = 0x20;
	self->P = 0;
	src = 0x20;
	clk = _BIT(self, &inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 3);
	assert_int_equal(self->P, 0x00);

	/* Test addressing mode clock */
	inst.opcode = Opcode_Get(0x2C); /* BIT ABS */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 4);
}

static void test_BMI(void **state) {
	test_BXX(state, 0x00, 0x80, 0x30, _BMI);
}

static void test_BNE(void **state) {
	test_BXX(state, 0x02, 0x00, 0xD0, _BNE);
}

static void test_BPL(void **state) {
	test_BXX(state, 0x80, 0x00, 0x10, _BPL);
}

static void test_BRK(void **state) {
	CPU *self = (CPU*) *state;
	Instruction inst;
	uint8_t (*ptr)(CPU*, Instruction*) = _BRK;
	uint8_t clk = 0, temp = 0xDC;

	/* Verify Opcode LUT */
	inst.opcode = Opcode_Get(0x00); /* BRK */
	assert_ptr_equal(ptr, inst.opcode.inst);

	/* Test BRK general behavior */
	/* Test : change PC */
	self->SP = 0xFF;
	self->PC = 0xABCC;
	self->P = 0x00;
	_STORE(self, 0xFFFE, &temp);
	temp = 0xFE;
	_STORE(self, 0xFFFF, &temp);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 7);
	assert_int_equal(self->PC, 0xFEDC);
	assert_int_equal(self->SP, 0xFC);
	assert_int_equal(0x10, _PULL(self));
	assert_int_equal(0xCD, _PULL(self));
	assert_int_equal(0xAB, _PULL(self));
	assert_int_equal(0x14, self->P);
}

static void test_BVC(void **state) {
	test_BXX(state, 0x40, 0x00, 0x50, _BVC);
}

static void test_BVS(void **state) {
	test_BXX(state, 0x00, 0x40, 0x70, _BVS);
}

static void test_JMP(void **state) {
	CPU *self = (CPU*) *state;
	Instruction inst;
	uint8_t (*ptr)(CPU*, Instruction*) = _JMP;
	uint8_t src[2] = {0xAA, 0xBB}, clk = 0;
	inst.dataMem = src;
	inst.pageCrossed = 1; /* Supposed to have no effect */

	/* Verify Opcode LUT */
	inst.opcode = Opcode_Get(0x4C); /* JMP ABS */
	assert_ptr_equal(ptr, inst.opcode.inst);

	/* Test JMP general behavior */
	/* Test : change PC */
	self->PC = 0x0000;
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 3);
	assert_int_equal(self->PC, 0xBBAA);

	/* Test addressing mode clock */
	inst.opcode = Opcode_Get(0x6C); /* JMP ABI */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 5);
}

static void test_JSR(void **state) {
	CPU *self = (CPU*) *state;
	Instruction inst;
	uint8_t (*ptr)(CPU*, Instruction*) = _JSR;
	uint8_t src[2] = {0xAA, 0xBB}, clk = 0;
	inst.dataMem = src;

	/* Verify Opcode LUT */
	inst.opcode = Opcode_Get(0x20); /* JSR */
	assert_ptr_equal(ptr, inst.opcode.inst);

	/* Test JSR general behavior */
	/* Test : change PC */
	inst.opcode.addressingMode = ABS;
	self->SP = 0xFF;
	self->PC = 0xABCE;
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 6);
	assert_int_equal(self->PC, 0xBBAA);
	assert_int_equal(self->SP, 0xFD);
	assert_int_equal(0xCD, _PULL(self));
	assert_int_equal(0xAB, _PULL(self));
}

static void test_CLC(void **state){
	CPU *self = (CPU*) *state;
	Instruction inst;
	uint8_t (*ptr)(CPU*, Instruction*) = _CLC;
	inst.opcode = Opcode_Get(0x18); /* CLC */
	uint8_t clk = inst.opcode.inst(self, &inst);
	self->SP = 0xFF;
	assert_int_equal(clk,2);
	assert_int_equal(((self->P)&0x01),0);
	assert_ptr_equal(ptr, inst.opcode.inst);
}

static void test_CLD(void **state){
	CPU *self = (CPU*) *state;
	Instruction inst;
	uint8_t (*ptr)(CPU*, Instruction*) = _CLD;
	inst.opcode = Opcode_Get(0xD8); /* CLD */
	uint8_t clk = inst.opcode.inst(self, &inst);
	self->SP = 0xFF;
	assert_int_equal(clk,2);
	assert_int_equal(((self->P)&0x08),0);
	assert_ptr_equal(ptr, inst.opcode.inst);
}

static void test_CLI(void **state){
	CPU *self = (CPU*) *state;
	Instruction inst;
	uint8_t (*ptr)(CPU*, Instruction*) = _CLI;
	inst.opcode = Opcode_Get(0x58); /* CLI */
	uint8_t clk = inst.opcode.inst(self, &inst);
	self->SP = 0xFF;
	assert_int_equal(clk,2);
	assert_int_equal(((self->P)&0x04),0);
	assert_ptr_equal(ptr, inst.opcode.inst);
}

static void test_CLV(void **state){
	CPU *self = (CPU*) *state;
	Instruction inst;
	uint8_t (*ptr)(CPU*, Instruction*) = _CLV;
	inst.opcode = Opcode_Get(0xB8); /* CLV */
	uint8_t clk = inst.opcode.inst(self, &inst);
	self->SP = 0xFF;
	assert_int_equal(clk,2);
	assert_int_equal(((self->P)&0x40),0);
	assert_ptr_equal(ptr, inst.opcode.inst);
}

static void test_CMP(void **state){
	CPU *self = (CPU*) *state;
	Instruction inst;
	uint8_t (*ptr)(CPU*, Instruction*) = _CMP;
	uint8_t src = 0x0, clk = 0;
	inst.pageCrossed = 0;
	self->A = 0x00;
	self->P = 0x00;
	inst.dataMem = &src;

	inst.opcode = Opcode_Get(0xC9); /* CMP Immediate clk=2 */
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk,2);
	assert_ptr_equal(ptr, inst.opcode.inst);

	inst.opcode = Opcode_Get(0xC5); /* CMP Zero-Page clk=3 */
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk,3);
	assert_ptr_equal(ptr, inst.opcode.inst);

	inst.opcode = Opcode_Get(0xD5); /* CMP Zero-Page X clk=4 */
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk,4);
	assert_ptr_equal(ptr, inst.opcode.inst);

	inst.opcode = Opcode_Get(0xCD); /* CMP Absolute clk=4 */
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk,4);
	assert_ptr_equal(ptr, inst.opcode.inst);

	inst.opcode = Opcode_Get(0xDD); /* CMP Absolute X clk=4* */
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk,4);
	assert_ptr_equal(ptr, inst.opcode.inst);

	inst.opcode = Opcode_Get(0xD9); /* CMP Absolute Y clk=4* */
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk,4);
	assert_ptr_equal(ptr, inst.opcode.inst);

	inst.opcode = Opcode_Get(0xC1); /* CMP Indirect X clk=6 */
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk,6);
	assert_ptr_equal(ptr, inst.opcode.inst);
	src = 0x00;
	inst.opcode = Opcode_Get(0xD1); /* CMP Indirect Y clk=5* */
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk,5);
	assert_ptr_equal(ptr, inst.opcode.inst);

	/*Test des bit des flags*/
	assert_int_equal(((self->P)&0x01),0x01);
	assert_int_equal(((self->P)&0x02),0x02);
	assert_int_equal(((self->P)&0x80),0x00);
}

static void test_CPX(void **state){
	CPU *self = (CPU*) *state;
	Instruction inst;
	uint8_t (*ptr)(CPU*, Instruction*) = _CPX;
	uint8_t src = 0x0, clk = 0;
	self->X = 0x80;
	self->P = 0x00;
	inst.dataMem = &src;

	inst.opcode = Opcode_Get(0xE0); /* _CPX Immediate clk=2 */
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk,2);
	assert_ptr_equal(ptr, inst.opcode.inst);

	inst.opcode = Opcode_Get(0xE4); /* _CPX Zero-page clk=3 */
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk,3);
	assert_ptr_equal(ptr, inst.opcode.inst);
	src = 0x90;
	inst.opcode = Opcode_Get(0xEC); /* _CPX Absolute clk=4 */
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk,4);
	assert_ptr_equal(ptr, inst.opcode.inst);

	assert_int_equal(((self->P)&0x01),0x00);
	assert_int_equal(((self->P)&0x02),0x00);
	assert_int_equal(((self->P)&0x80),0x80);

}

static void test_CPY(void **state){
	CPU *self = (CPU*) *state;
	Instruction inst;
	uint8_t (*ptr)(CPU*, Instruction*) = _CPY;
	uint8_t src = 0x0, clk = 0;
	self->Y = 0x60;
	self->P = 0x00;
	inst.dataMem = &src;

	inst.opcode = Opcode_Get(0xC0); /* _CPY Immediate clk=2 */
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk,2);
	assert_ptr_equal(ptr, inst.opcode.inst);

	inst.opcode = Opcode_Get(0xC4); /* _CPY Zero-page clk=3 */
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk,3);
	assert_ptr_equal(ptr, inst.opcode.inst);
	src = 0x00;
	inst.opcode = Opcode_Get(0xCC); /* _CPY Absolute clk=4 */
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk,4);
	assert_ptr_equal(ptr, inst.opcode.inst);

	assert_int_equal(((self->P)&0x01),0x01);
	assert_int_equal(((self->P)&0x02),0x00);
	assert_int_equal(((self->P)&0x80),0x00);

}

static void test_DEC(void **state){
	CPU *self = (CPU*) *state;
	Instruction inst;
	uint8_t (*ptr)(CPU*, Instruction*) = _DEC;
	uint8_t src = 0x01, clk = 0;
	inst.dataMem = &src;

	inst.opcode = Opcode_Get(0xC6); /* _DEC Zero page clk=5 */
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk,5);
	assert_ptr_equal(ptr, inst.opcode.inst);

	inst.opcode = Opcode_Get(0xD6); /* _DEC Zero-page X clk=6 */
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk,6);
	assert_ptr_equal(ptr, inst.opcode.inst);

	inst.opcode = Opcode_Get(0xCE); /* _DEC Absolute clk=6 */
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk,6);
	assert_ptr_equal(ptr, inst.opcode.inst);

	src = 0x00;
	inst.opcode = Opcode_Get(0xDE); /* _DEC Absolute X clk=7 */
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk,7);
	assert_ptr_equal(ptr, inst.opcode.inst);

	assert_int_equal(src,0XFF);
	assert_int_equal(((self->P)&0x02),0x00);
	assert_int_equal(((self->P)&0x80),0x80);
}

static void test_DEX(void **state){
	CPU *self = (CPU*) *state;
	Instruction inst;
	uint8_t (*ptr)(CPU*, Instruction*) = _DEX;
	uint8_t clk = 0;
	self->X = 0x01;

	inst.opcode = Opcode_Get(0xCA); /* _DEX Implied clk=2 */
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk,2);
	assert_ptr_equal(ptr, inst.opcode.inst);

	assert_int_equal(self->X,0x00);
	assert_int_equal(((self->P)&0x02),0x02);
	assert_int_equal(((self->P)&0x80),0x00);
}

static void test_DEY(void **state){
	CPU *self = (CPU*) *state;
	Instruction inst;
	uint8_t (*ptr)(CPU*, Instruction*) = _DEY;
	uint8_t clk = 0;
	self->Y = 0x00;

	inst.opcode = Opcode_Get(0x88); /* _DEY Implied clk=2 */
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk,2);
	assert_ptr_equal(ptr, inst.opcode.inst);

	assert_int_equal(self->Y,0xFF);
	assert_int_equal(((self->P)&0x02),0x00);
	assert_int_equal(((self->P)&0x80),0x80);
}

static void test_EOR(void **state){
	CPU *self = (CPU*) *state;
	Instruction inst;
	uint8_t (*ptr)(CPU*, Instruction*) = _EOR;
	uint8_t src = 0x00, clk = 0;
	inst.pageCrossed = 0;
	self->A = 0x00;
	self->P = 0x00;
	inst.dataMem = &src;

	inst.opcode = Opcode_Get(0x49); /* _EOR Immediate clk=2 */
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk,2);
	assert_ptr_equal(ptr, inst.opcode.inst);

	inst.opcode = Opcode_Get(0x45); /* _EOR Zero-page clk=3 */
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk,3);
	assert_ptr_equal(ptr, inst.opcode.inst);

	inst.opcode = Opcode_Get(0x55); /* _EOR Zero-page X clk=4 */
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk,4);
	assert_ptr_equal(ptr, inst.opcode.inst);

	inst.opcode = Opcode_Get(0x4D); /* _EOR Absolute clk=4 */
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk,4);
	assert_ptr_equal(ptr, inst.opcode.inst);

	inst.opcode = Opcode_Get(0x5D); /* _EOR Absolute X clk=4* */
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk,4);
	assert_ptr_equal(ptr, inst.opcode.inst);

	inst.opcode = Opcode_Get(0x59); /* _EOR Absolute Y clk=4* */
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk,4);
	assert_ptr_equal(ptr, inst.opcode.inst);

	inst.opcode = Opcode_Get(0x41); /* _EOR Indirect X clk=6 */
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk,6);
	assert_ptr_equal(ptr, inst.opcode.inst);
	src = 0xFF;
	inst.opcode = Opcode_Get(0x51); /* _EOR Indirect Y clk=5* */
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk,5);
	assert_ptr_equal(ptr, inst.opcode.inst);

	assert_int_equal(self->A,0xFF);
	assert_int_equal(((self->P)&0x02),0x00);
	assert_int_equal(((self->P)&0x80),0x80);
}

static void test_INC(void **state){
	CPU *self = (CPU*) *state;
	Instruction inst;
	uint8_t (*ptr)(CPU*, Instruction*) = _INC;
	uint8_t src = 0x01, clk = 0;
	inst.dataMem = &src;

	inst.opcode = Opcode_Get(0xE6); /* _INC Zero page clk=5 */
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk,5);
	assert_ptr_equal(ptr, inst.opcode.inst);

	inst.opcode = Opcode_Get(0xF6); /* _INC Zero-page X clk=6 */
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk,6);
	assert_ptr_equal(ptr, inst.opcode.inst);

	inst.opcode = Opcode_Get(0xEE); /* _INC Absolute clk=6 */
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk,6);
	assert_ptr_equal(ptr, inst.opcode.inst);

	src = 0xFF;
	inst.opcode = Opcode_Get(0xFE); /* _INC Absolute X clk=7 */
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk,7);
	assert_ptr_equal(ptr, inst.opcode.inst);

	assert_int_equal(src,0X00);
	assert_int_equal(((self->P)&0x02),0x02);
	assert_int_equal(((self->P)&0x80),0x00);
}

static void test_INX(void **state){
	CPU *self = (CPU*) *state;
	Instruction inst;
	uint8_t (*ptr)(CPU*, Instruction*) = _INX;
	uint8_t clk = 0;
	self->X = 0xFE;

	inst.opcode = Opcode_Get(0xE8); /* _INX Implied clk=2 */
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk,2);
	assert_ptr_equal(ptr, inst.opcode.inst);

	assert_int_equal(self->X,0xFF);
	assert_int_equal(((self->P)&0x02),0x00);
	assert_int_equal(((self->P)&0x80),0x80);
}

static void test_INY(void **state){
	CPU *self = (CPU*) *state;
	Instruction inst;
	uint8_t (*ptr)(CPU*, Instruction*) = _INY;
	uint8_t clk = 0;
	self->Y = 0xFE;

	inst.opcode = Opcode_Get(0xC8); /* _INY Implied clk=2 */
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk,2);
	assert_ptr_equal(ptr, inst.opcode.inst);

	assert_int_equal(self->Y,0xFF);
	assert_int_equal(((self->P)&0x02),0x00);
	assert_int_equal(((self->P)&0x80),0x80);
}

static void test_LDA(void **state){
	CPU *self = (CPU*) *state;
	Instruction inst;
	uint8_t (*ptr)(CPU*, Instruction*) = _LDA;
	uint8_t src = 0x00, clk = 0;
	inst.dataMem = &src;
	inst.pageCrossed = 0;

	/* Verify opcode LUT */
	inst.opcode = Opcode_Get(0xA9); /* LDA IMM */
	assert_ptr_equal(ptr, inst.opcode.inst);
	/* Test LDA general behavior */
	/* Signed and Non-Zero */
	self->P = 0;
	src = 0xFF;
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 2);
	assert_int_equal(self->P & 0x80,0x80);
	assert_int_equal(self->P & 0x02,0x00);
	assert_int_equal(self->A,0xFF);
	/* Unsigned and Non-Zero */
	self->P = 0;
	src = 0x04;
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 2);
	assert_int_equal(self->P & 0x80,0x00);
	assert_int_equal(self->P & 0x02,0x00);
	assert_int_equal(self->A,0x04);
	/* Unsigned and Zero*/
	self->P = 0;
	src = 0x00;
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 2);
	assert_int_equal(self->P & 0x80,0x00);
	assert_int_equal(self->P & 0x02,0x02);
	assert_int_equal(self->A,0x00);

	/* Test addressing mode clock */
	inst.opcode = Opcode_Get(0xA5); /* LDA ZER */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 3);

	inst.opcode = Opcode_Get(0xB5); /* LDA ZEX */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 4);

	inst.opcode = Opcode_Get(0xAD); /* LDA ABS */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 4);

	inst.pageCrossed = 0;
	inst.opcode = Opcode_Get(0xBD); /* LDA ABX */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 4);

	inst.pageCrossed = 1;
	inst.opcode = Opcode_Get(0xBD); /* LDA ABX */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 5);

	inst.pageCrossed = 0;
	inst.opcode = Opcode_Get(0xB9); /* LDA ABY */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 4);

	inst.pageCrossed = 1;
	inst.opcode = Opcode_Get(0xB9); /* LDA ABY */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 5);

	inst.pageCrossed = 0;
	inst.opcode = Opcode_Get(0xA1); /* LDA INX */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 6);

	inst.pageCrossed = 0;
	inst.opcode = Opcode_Get(0xB1); /* LDA INY */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 5);

	inst.pageCrossed = 1;
	inst.opcode = Opcode_Get(0xB1); /* LDA INY */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 6);
}

static void test_LDX(void **state){
	CPU *self = (CPU*) *state;
	Instruction inst;
	uint8_t (*ptr)(CPU*, Instruction*) = _LDX;
	uint8_t src = 0x00, clk = 0;
	inst.dataMem = &src;
	inst.pageCrossed = 0;

	/* Verify opcode LUT */
	inst.opcode = Opcode_Get(0xA2); /* LDX IMM */
	assert_ptr_equal(ptr, inst.opcode.inst);
	/* Test LDX general behavior */
	/* Signed and Non-Zero */
	self->P = 0;
	src = 0xFF;
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 2);
	assert_int_equal(self->P & 0x80,0x80);
	assert_int_equal(self->P & 0x02,0x00);
	assert_int_equal(self->X,0xFF);
	/* Unsigned and Non-Zero */
	self->P = 0;
	src = 0x04;
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 2);
	assert_int_equal(self->P & 0x80,0x00);
	assert_int_equal(self->P & 0x02,0x00);
	assert_int_equal(self->X,0x04);
	/* Unsigned and Zero*/
	self->P = 0;
	src = 0x00;
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 2);
	assert_int_equal(self->P & 0x80,0x00);
	assert_int_equal(self->P & 0x02,0x02);
	assert_int_equal(self->X,0x00);

	/* Test addressing mode clock */
	inst.opcode = Opcode_Get(0xA6); /* LDX ZER */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 3);

	inst.opcode = Opcode_Get(0xB6); /* LDX ZEY */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 4);

	inst.opcode = Opcode_Get(0xAE); /* LDX ABS */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 4);

	inst.pageCrossed = 0;
	inst.opcode = Opcode_Get(0xBE); /* LDX ABY */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 4);

	inst.pageCrossed = 1;
	inst.opcode = Opcode_Get(0xBE); /* LDX ABY */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 5);
}

static void test_LDY(void **state){
	CPU *self = (CPU*) *state;
	Instruction inst;
	uint8_t (*ptr)(CPU*, Instruction*) = _LDY;
	uint8_t src = 0x00, clk = 0;
	inst.dataMem = &src;
	inst.pageCrossed = 0;

	/* Verify opcode LUT */
	inst.opcode = Opcode_Get(0xA0); /* LDX IMM */
	assert_ptr_equal(ptr, inst.opcode.inst);
	/* Test LDX general behavior */
	/* Signed and Non-Zero */
	self->P = 0;
	src = 0xFF;
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 2);
	assert_int_equal(self->P & 0x80,0x80);
	assert_int_equal(self->P & 0x02,0x00);
	assert_int_equal(self->Y,0xFF);
	/* Unsigned and Non-Zero */
	self->P = 0;
	src = 0x04;
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 2);
	assert_int_equal(self->P & 0x80,0x00);
	assert_int_equal(self->P & 0x02,0x00);
	assert_int_equal(self->Y,0x04);
	/* Unsigned and Zero*/
	self->P = 0;
	src = 0x00;
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 2);
	assert_int_equal(self->P & 0x80,0x00);
	assert_int_equal(self->P & 0x02,0x02);
	assert_int_equal(self->Y,0x00);

	/* Test addressing mode clock */
	inst.opcode = Opcode_Get(0xA4); /* LDY ZER */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 3);

	inst.opcode = Opcode_Get(0xB4); /* LDY ZEX */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 4);

	inst.opcode = Opcode_Get(0xAC); /* LDY ABS */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 4);

	inst.pageCrossed = 0;
	inst.opcode = Opcode_Get(0xBC); /* LDY ABX */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 4);

	inst.pageCrossed = 1;
	inst.opcode = Opcode_Get(0xBC); /* LDY ABX */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 5);
}

static void test_LSR(void **state){
	CPU *self = (CPU*) *state;
	Instruction inst;
	uint8_t (*ptr)(CPU*, Instruction*) = _LSR;
	uint8_t src = 0x00, clk = 0;
	inst.dataMem = &src;
	inst.pageCrossed = 0;

	/* Verify opcode LUT */
	inst.opcode = Opcode_Get(0x4A); /* LSR ACC */
	assert_ptr_equal(ptr, inst.opcode.inst);
	/* Test LDX general behavior */
	/* Signed and Non-Zero */
	self->P = 0;
	src = 0xFF;
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 2);
	assert_int_equal(self->P & 0x80,0x00);
	assert_int_equal(self->P & 0x02,0x00);
	assert_int_equal(self->P & 0x01,0x01);
	assert_int_equal(src,0x7F);
	/* Unsigned and Non-Zero */
	self->P = 0;
	src = 0x04;
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 2);
	assert_int_equal(self->P & 0x80,0x00);
	assert_int_equal(self->P & 0x02,0x00);
	assert_int_equal(self->P & 0x01,0x00);
	assert_int_equal(src,0x02);
	/* Unsigned and Zero*/
	self->P = 0;
	src = 0x00;
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 2);
	assert_int_equal(self->P & 0x80,0x00);
	assert_int_equal(self->P & 0x02,0x02);
	assert_int_equal(self->P & 0x01,0x00);
	assert_int_equal(src,0x00);

	/* Test addressing mode clock */
	inst.opcode = Opcode_Get(0x46); /* LSR ZER */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 5);

	inst.opcode = Opcode_Get(0x56); /* LSR ZEX */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 6);

	inst.opcode = Opcode_Get(0x4E); /* LSR ABS */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 6);

	inst.opcode = Opcode_Get(0x5E); /* LSR ABX */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 7);
}

static void test_NOP(void **state){
	CPU *self = (CPU*) *state;
	Instruction inst;
	uint8_t (*ptr)(CPU*, Instruction*) = _NOP;
	uint8_t clk = 0;

	/* Verify opcode LUT */
	inst.opcode = Opcode_Get(0xEA); /* NOP IMP */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 2);
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
		cmocka_unit_test(test_BIT),
		cmocka_unit_test(test_BMI),
		cmocka_unit_test(test_BNE),
		cmocka_unit_test(test_BPL),
		cmocka_unit_test(test_BRK),
		cmocka_unit_test(test_BVC),
		cmocka_unit_test(test_BVS),
		cmocka_unit_test(test_JMP),
		cmocka_unit_test(test_JSR),
		cmocka_unit_test(test_CLC),
		cmocka_unit_test(test_CLD),
		cmocka_unit_test(test_CLI),
		cmocka_unit_test(test_CLV),
		cmocka_unit_test(test_CMP),
		cmocka_unit_test(test_CPX),
		cmocka_unit_test(test_CPY),
		cmocka_unit_test(test_DEC),
		cmocka_unit_test(test_DEX),
		cmocka_unit_test(test_DEY),
		cmocka_unit_test(test_EOR),
		cmocka_unit_test(test_INC),
		cmocka_unit_test(test_INX),
		cmocka_unit_test(test_INY),
		cmocka_unit_test(test_LDA),
		cmocka_unit_test(test_LDX),
		cmocka_unit_test(test_LDY),
		cmocka_unit_test(test_LSR),
		cmocka_unit_test(test_NOP),
		};
	const struct CMUnitTest test_addressing_Mode[] = {
		cmocka_unit_test(test_addressing_IMP),
		cmocka_unit_test(test_addressing_ACC),
		cmocka_unit_test(test_addressing_ZEX),
		cmocka_unit_test(test_addressing_ZEY),
		cmocka_unit_test(test_addressing_INX),
		cmocka_unit_test(test_addressing_ZEY),
		cmocka_unit_test(test_addressing_INY),
		cmocka_unit_test(test_addressing_IMM),
		cmocka_unit_test(test_addressing_ZER),
		cmocka_unit_test(test_addressing_REL),
		cmocka_unit_test(test_addressing_ABS),
		cmocka_unit_test(test_addressing_ABX),
		cmocka_unit_test(test_addressing_ABY),
		cmocka_unit_test(test_addressing_ABI),
		cmocka_unit_test(test_addressing_MIS),
	};
	const struct CMUnitTest test_fetch[] = {
		cmocka_unit_test(test_instruction_fetch),
	};
	int out = 0;
	out += cmocka_run_group_tests(test_instruction_macro, setup_CPU, teardown_CPU);
	out += cmocka_run_group_tests(test_instruction, setup_CPU, teardown_CPU);
	out += cmocka_run_group_tests(test_addressing_Mode, setup_CPU, teardown_CPU);
	out += cmocka_run_group_tests(test_fetch, setup_CPU, teardown_CPU);
	return out;
}

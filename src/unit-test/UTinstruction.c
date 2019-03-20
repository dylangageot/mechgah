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
	assert_int_equal(instru->nbArg, 0);
	/* 0xB5 -> opcode d'une instruction utilisant comme m_d ZEX*/
	self->PC = 0x80F0;
	*(mapper->get(mapper->memoryMap, AS_CPU, 0x80F0)) = 0xB5;
	assert_int_equal(Instruction_Fetch(instru,self),1);
	assert_int_equal(instru->nbArg, 1);
	assert_int_equal(instru->opcodeArg[0],0x12);
	/* 0x98 -> opcode d'une instruction utilisant comme m_d ABX*/
	self->PC = 0x80F0;
	*(mapper->get(mapper->memoryMap, AS_CPU, 0x80F0)) = 0xD9;
	assert_int_equal(Instruction_Fetch(instru,self),1);
	assert_int_equal(instru->nbArg, 2);
	assert_int_equal(instru->opcodeArg[0],0x12);
	assert_int_equal(instru->opcodeArg[1],0x13);
	/* 0x03 -> opcode inexistant*/
	self->PC = 0x80F0;
	*(mapper->get(mapper->memoryMap, AS_CPU, 0x80F0)) = 0x03;
	assert_int_equal(Instruction_Fetch(NULL,NULL),0);
	assert_int_equal(Instruction_Fetch(instru,self),0);
	assert_int_equal(instru->nbArg, 0);
	assert_int_equal(Instruction_Fetch(NULL,NULL),0);

	free(instru);
}

static void test_Instruction_PrintLog(void **state) {
	CPU *self = (CPU*) *state;
	Mapper *mapper = self->rmap;
	Instruction inst;
	char expectedStr[] =
		"8000 6D CD AB    A:11 X:22 Y:33 P:44 SP:55 CYC:555\n";
	char readStr[256];
	FILE *fLog = NULL;
	uint8_t *memory = mapper->get(mapper->memoryMap, AS_CPU, 0x8000);
	memory[0] = 0x6D;
	memory[1] = 0xCD;
	memory[2] = 0xAB;
	self->PC = 0x8000;
	self->A = 0x11;
	self->X = 0x22;
	self->Y = 0x33;
	self->P = 0x44;
	self->SP = 0x55;
	assert_int_equal(Instruction_Fetch(&inst, self), 1);
	remove("cpu.log");
	Instruction_PrintLog(&inst, self, 555);
	fLog = fopen("cpu.log", "r");
	assert_ptr_not_equal(fLog, NULL);
	fgets(readStr, 256, fLog);
	assert_int_equal(strcmp(expectedStr,readStr), 0);
	fclose(fLog);
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

static void test_AND(void **state) {
	CPU *self = (CPU*) *state;
	Instruction inst;
	uint8_t (*ptr)(CPU*, Instruction*) = _AND;
	uint8_t src = 0xFF, clk = 0;
	inst.dataMem = &src;
	inst.pageCrossed = 0;
	self->A = 0x00;
	self->P = 0x00;

	inst.opcode = Opcode_Get(0x29); /* _AND Immediate clk=2 */
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk,2);
	assert_ptr_equal(ptr, inst.opcode.inst);

	inst.opcode = Opcode_Get(0x25); /* _AND Zero Page  clk=3 */
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk,3);
	assert_ptr_equal(ptr, inst.opcode.inst);

	inst.opcode = Opcode_Get(0x35); /* _AND Zero Page,X  clk=4 */
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk,4); //Erreur sur le nombre de cycle d'horloge
	assert_ptr_equal(ptr, inst.opcode.inst);

	inst.opcode = Opcode_Get(0x2D); /* _AND Absolute clk=4 */
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk,4);
	assert_ptr_equal(ptr, inst.opcode.inst);

	inst.opcode = Opcode_Get(0x3D); /* _AND Absolute,X  clk=4 */
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk,4);
	assert_ptr_equal(ptr, inst.opcode.inst);

	inst.opcode = Opcode_Get(0x39); /* _AND Absolute,Y  clk=4 */
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk,4);
	assert_ptr_equal(ptr, inst.opcode.inst);

	inst.opcode = Opcode_Get(0x21); /* _AND Indirect,X  clk=6 */
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk,6);
	assert_ptr_equal(ptr, inst.opcode.inst);

	self->A = 0x81;

	inst.opcode = Opcode_Get(0x31); /* _AND Indirect,Y  clk=5 */
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk,5);
	assert_ptr_equal(ptr, inst.opcode.inst);

	assert_int_equal(self->A,0x81);
	assert_int_equal(self->P&0x02,0x00);
	assert_int_equal(self->P&0x80,0x80);
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
	uint8_t clk = 0;
	inst.dataAddr = 0xBBAA;
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
	uint8_t clk = 0;
	inst.dataAddr = 0xBBAA;

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

static void test_PHA(void **state) {
	CPU* self = (CPU*) *state;
	Instruction inst;
	uint8_t (*ptr)(CPU*, Instruction*) = _PHA;
	uint8_t clock = 0;

	/* Verify Opcode */
	inst.opcode = Opcode_Get(0x48); /* PHA */
	assert_ptr_equal(ptr, inst.opcode.inst);

	/* Test PHA behaviour */
	self->A = 0x8F;
	clock = inst.opcode.inst(self,&inst);

	assert_int_equal(clock, 3);
	assert_int_equal(_PULL(self), 0X8F);
}

static void test_PHP(void **state) {
	CPU* self = (CPU*) *state;
	Instruction inst;
	uint8_t (*ptr)(CPU*, Instruction*)  = _PHP;
	uint8_t clock = 0;

	/* Verify Opcode */
	inst.opcode = Opcode_Get(0x08); /* PHP */
	assert_ptr_equal(ptr, inst.opcode.inst);

	/* Test PHP behaviour */
	self->P = 0x2A;
	clock = inst.opcode.inst(self,&inst);

	assert_int_equal(clock, 3);
	assert_int_equal(_PULL(self), 0X2A);
}

static void test_PLA(void **state) {
	CPU* self = (CPU*) *state;
	Instruction inst;
	uint8_t (*ptr)(CPU*, Instruction*)  = _PLA;
	uint8_t clock = 0;

	/* Verify Opcode */
	inst.opcode = Opcode_Get(0x68); /* PLA */
	assert_ptr_equal(ptr, inst.opcode.inst);

	/* Test PLA behaviour */

	uint8_t test_value = 0xFF;
	_SET_SR(self, &test_value);

	/* POSITIVE VALUE */
	test_value = 0x35;
	_PUSH(self, &test_value);

	clock = inst.opcode.inst(self,&inst);

	assert_int_equal(clock, 4);
	assert_int_equal(self->A, 0X35);

	/* N flag clear */
	uint8_t sr = _GET_SR(self);
	assert_int_equal((sr >> 7) & 1UL, 0);

	/* Z flag clear */
	assert_int_equal((sr >> 1) & 1UL, 0);


	/* NEGATIVE VALUE */
	test_value = 0x8C;
	_PUSH(self, &test_value);

	clock = inst.opcode.inst(self,&inst);

	assert_int_equal(clock, 4);
	assert_int_equal(self->A, 0X8C);

	/* N flag set */
	sr = _GET_SR(self);
	assert_int_equal((sr >> 7) & 1UL, 1);

	/* Z flag clear */
	assert_int_equal((sr >> 1) & 1UL, 0);


	/* ZERO VALUE */
	test_value = 0;
	_PUSH(self, &test_value);

	clock = inst.opcode.inst(self,&inst);

	assert_int_equal(clock, 4);
	assert_int_equal(self->A, 0);

	/* N flag clear */
	sr = _GET_SR(self);
	assert_int_equal((sr >> 7) & 1UL, 0);

	/* Z flag set */
	assert_int_equal((sr >> 1) & 1UL, 1);
}

static void test_PLP(void **state) {
	CPU* self = (CPU*) *state;
	Instruction inst;
	uint8_t (*ptr)(CPU*, Instruction*)  = _PLP;
	uint8_t clock = 0;

	/* Verify Opcode */
	inst.opcode = Opcode_Get(0x28); /* PLP */
	assert_ptr_equal(ptr, inst.opcode.inst);

	/* Test PLP behaviour */
	uint8_t test_value = 0xD7;
	_PUSH(self, &test_value);

	clock = inst.opcode.inst(self,&inst);

	assert_int_equal(clock, 4);
	assert_int_equal(_GET_SR(self), 0xD7);

}

static void test_RTI(void **state) {
	CPU* self = (CPU*) *state;
	Instruction inst;
	uint8_t (*ptr)(CPU*, Instruction*)  = _RTI;
	uint8_t clock = 0;

	/* Verify Opcode */
	inst.opcode = Opcode_Get(0x40); /* RTI */
	assert_ptr_equal(ptr, inst.opcode.inst);

	/* Test RTI behaviour */
	uint8_t test_value = 0xD7;
	_SET_SR(self, &test_value);

	self->PC = 0xF56D;

	/* push SR and PC on stack */
	uint8_t temp = (self->PC >> 8) & 0xFF;
	_PUSH(self, &temp);
	temp = self->PC & 0xFF;
	_PUSH(self, &temp);
	_PUSH(self, &self->P);

	/* change values of SR and PC */
	test_value = 0x6B;
	_SET_SR(self, &test_value);

	self->PC = 0x1234;

	clock = inst.opcode.inst(self,&inst);

	assert_int_equal(clock, 6);
	assert_int_equal(_GET_SR(self), 0xD7);
	assert_int_equal(self->PC, 0xF56D);
}

static void test_RTS(void **state) {
	CPU* self = (CPU*) *state;
	Instruction inst;
	uint8_t (*ptr)(CPU*, Instruction*)  = _RTS;
	uint8_t clock = 0;

	/* Verify Opcode */
	inst.opcode = Opcode_Get(0x60); /* RTS */
	assert_ptr_equal(ptr, inst.opcode.inst);

	/* Test RTS behaviour */
	self->PC = 0xA731;

	/* push PC on stack */
	uint8_t temp = (self->PC >> 8) & 0xFF;
	_PUSH(self, &temp);
	temp = self->PC & 0xFF;
	_PUSH(self, &temp);

	/* change value of PC */
	self->PC = 0x9B03;

	clock = inst.opcode.inst(self,&inst);

	assert_int_equal(clock, 6);
	assert_int_equal(self->PC, 0xA731);
}

static void test_SEI(void **state) {
	CPU* self = (CPU*) *state;
	Instruction inst;
	uint8_t (*ptr)(CPU*, Instruction*)  = _SEI;
	uint8_t clock = 0;

	/* Verify Opcode */
	inst.opcode = Opcode_Get(0x78); /* SEI */
	assert_ptr_equal(ptr, inst.opcode.inst);

	/* Test SEI behaviour */

	/* clear I flag */
	self->P &= ~(1UL << 2);

	clock = inst.opcode.inst(self,&inst);

	assert_int_equal(clock, 2);
	assert_int_equal(self->P & 0x04, 0x04);
}

static void test_TAX(void **state) {
	CPU* self = (CPU*) *state;
	Instruction inst;
	uint8_t (*ptr)(CPU*, Instruction*)  = _TAX;
	uint8_t clock = 0;

	/* Verify Opcode */
	inst.opcode = Opcode_Get(0xAA); /* TAX */
	assert_ptr_equal(ptr, inst.opcode.inst);

	/* Test TAX behaviour */

	/* POSITIVE VALUE */
	self->A = 0x45;

	clock = inst.opcode.inst(self,&inst);

	assert_int_equal(clock, 2);
	assert_int_equal(self->X, 0X45);

	/* N flag clear */
	uint8_t sr = _GET_SR(self);
	assert_int_equal((sr >> 7) & 1UL, 0);

	/* Z flag clear */
	assert_int_equal((sr >> 1) & 1UL, 0);


	/* NEGATIVE VALUE */
	self->A = 0xF3;

	clock = inst.opcode.inst(self,&inst);

	assert_int_equal(clock, 2);
	assert_int_equal(self->X, 0XF3);

	/* N flag set */
	sr = _GET_SR(self);
	assert_int_equal((sr >> 7) & 1UL, 1);

	/* Z flag clear */
	assert_int_equal((sr >> 1) & 1UL, 0);


	/* ZERO VALUE */
	self->A = 0;

	clock = inst.opcode.inst(self,&inst);

	assert_int_equal(clock, 2);
	assert_int_equal(self->X, 0x0);

	/* N flag clear */
	sr = _GET_SR(self);
	assert_int_equal((sr >> 7) & 1UL, 0);

	/* Z flag set */
	assert_int_equal((sr >> 1) & 1UL, 1);

}

static void test_TAY(void **state) {
	CPU* self = (CPU*) *state;
	Instruction inst;
	uint8_t (*ptr)(CPU*, Instruction*)  = _TAY;
	uint8_t clock = 0;

	/* Verify Opcode */
	inst.opcode = Opcode_Get(0xA8); /* TAY */
	assert_ptr_equal(ptr, inst.opcode.inst);

	/* Test TAY behaviour */

	/* POSITIVE VALUE */
	self->A = 0x56;

	clock = inst.opcode.inst(self,&inst);

	assert_int_equal(clock, 2);
	assert_int_equal(self->Y, 0X56);

	/* N flag clear */
	uint8_t sr = _GET_SR(self);
	assert_int_equal((sr >> 7) & 1UL, 0);

	/* Z flag clear */
	assert_int_equal((sr >> 1) & 1UL, 0);


	/* NEGATIVE VALUE */
	self->A = 0xF3;

	clock = inst.opcode.inst(self,&inst);

	assert_int_equal(clock, 2);
	assert_int_equal(self->Y, 0XF3);

	/* N flag set */
	sr = _GET_SR(self);
	assert_int_equal((sr >> 7) & 1UL, 1);

	/* Z flag clear */
	assert_int_equal((sr >> 1) & 1UL, 0);


	/* ZERO VALUE */
	self->A = 0;

	clock = inst.opcode.inst(self,&inst);

	assert_int_equal(clock, 2);
	assert_int_equal(self->Y, 0x0);

	/* N flag clear */
	sr = _GET_SR(self);
	assert_int_equal((sr >> 7) & 1UL, 0);

	/* Z flag set */
	assert_int_equal((sr >> 1) & 1UL, 1);

}

static void test_TSX(void **state) {
	CPU* self = (CPU*) *state;
	Instruction inst;
	uint8_t (*ptr)(CPU*, Instruction*)  = _TSX;
	uint8_t clock = 0;

	/* Verify Opcode */
	inst.opcode = Opcode_Get(0xBA); /* TSX */
	assert_ptr_equal(ptr, inst.opcode.inst);

	/* Test TSX behaviour */

	/* POSITIVE VALUE */
	self->SP = 0x45;

	clock = inst.opcode.inst(self,&inst);

	assert_int_equal(clock, 2);
	assert_int_equal(self->X, 0X45);

	/* N flag clear */
	uint8_t sr = _GET_SR(self);
	assert_int_equal((sr >> 7) & 1UL, 0);

	/* Z flag clear */
	assert_int_equal((sr >> 1) & 1UL, 0);


	/* NEGATIVE VALUE */
	self->SP = 0xF3;

	clock = inst.opcode.inst(self,&inst);

	assert_int_equal(clock, 2);
	assert_int_equal(self->X, 0XF3);

	/* N flag set */
	sr = _GET_SR(self);
	assert_int_equal((sr >> 7) & 1UL, 1);

	/* Z flag clear */
	assert_int_equal((sr >> 1) & 1UL, 0);


	/* ZERO VALUE */
	self->SP = 0;

	clock = inst.opcode.inst(self,&inst);

	assert_int_equal(clock, 2);
	assert_int_equal(self->X, 0x0);

	/* N flag clear */
	sr = _GET_SR(self);
	assert_int_equal((sr >> 7) & 1UL, 0);

	/* Z flag set */
	assert_int_equal((sr >> 1) & 1UL, 1);

}

static void test_SEC(void **state){
	CPU *self = (CPU*) *state;
	Instruction inst;
	uint8_t (*ptr)(CPU*, Instruction*) = _SEC;
	uint8_t clk = 0;
	self->P = 0x00;

	/* Verify opcode LUT */
	inst.opcode = Opcode_Get(0x38); /* SEC IMP */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 2);

	/* Verify general behavior */
	assert_int_equal(self->P & 0x01,0x01);
}

static void test_SED(void **state){
	CPU *self = (CPU*) *state;
	Instruction inst;
	uint8_t (*ptr)(CPU*, Instruction*) = _SED;
	uint8_t clk = 0;
	self->P = 0x00;

	/* Verify opcode LUT */
	inst.opcode = Opcode_Get(0xF8); /* SED IMP */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 2);

	/* Verify general behavior */
	assert_int_equal(self->P & 0x08,0x08);
}

static void test_STA(void **state){
	CPU *self = (CPU*) *state;
	Instruction inst;
	uint8_t (*ptr)(CPU*, Instruction*) = _STA;
	self->A = 0x11;
	uint8_t src = 0xFF, clk = 0;
	inst.dataMem = &src;

	/* Verify opcode LUT */
	inst.opcode = Opcode_Get(0x85); /* STA ZER */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 3);

	/* Verify general behavior */
	assert_int_equal(*(inst.dataMem),self->A);

	/* Test addressing mode clock */
	inst.opcode = Opcode_Get(0x95); /* STA ZEX */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 4);

	inst.opcode = Opcode_Get(0x8D); /* STA ABS */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 4);

	inst.opcode = Opcode_Get(0x9D); /* LSR ABX */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 5);

	inst.opcode = Opcode_Get(0x99); /* LSR ABY */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 5);

	inst.opcode = Opcode_Get(0x81); /* LSR INX */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 6);

	inst.opcode = Opcode_Get(0x91); /* LSR INY */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 6);
}

static void test_STX(void **state){
	CPU *self = (CPU*) *state;
	Instruction inst;
	uint8_t (*ptr)(CPU*, Instruction*) = _STX;
	self->X = 0x11;
	uint8_t src = 0xFF, clk = 0;
	inst.dataMem = &src;

	/* Verify opcode LUT */
	inst.opcode = Opcode_Get(0x86); /* STX ZER */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 3);

	/* Verify general behavior */
	assert_int_equal(*(inst.dataMem),self->X);

	/* Test addressing mode clock */
	inst.opcode = Opcode_Get(0x96); /* STX ZEY */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 4);

	inst.opcode = Opcode_Get(0x8E); /* STX ABS */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 4);
}

static void test_STY(void **state){
	CPU *self = (CPU*) *state;
	Instruction inst;
	uint8_t (*ptr)(CPU*, Instruction*) = _STY;
	self->Y = 0x11;
	uint8_t src = 0xFF, clk = 0;
	inst.dataMem = &src;

	/* Verify opcode LUT */
	inst.opcode = Opcode_Get(0x84); /* STY ZER */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 3);

	/* Verify general behavior */
	assert_int_equal(*(inst.dataMem),self->Y);

	/* Test addressing mode clock */
	inst.opcode = Opcode_Get(0x94); /* STY ZEX */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 4);

	inst.opcode = Opcode_Get(0x8C); /* STY ABS */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 4);
}

static void test_ORA(void **state){
	CPU *self = (CPU*) *state;
	Instruction inst;
	uint8_t (*ptr)(CPU*, Instruction*) = _ORA;
	uint8_t src = 0x00, clk = 0;
	inst.dataMem = &src;
	inst.pageCrossed = 0;

	/* Verify opcode LUT */
	inst.opcode = Opcode_Get(0x09); /* ORA IMM */
	assert_ptr_equal(ptr, inst.opcode.inst);
	/* Test ORA general behavior */
	/* Result is signed and non zero */
	self->P = 0;
	self->A = 0x69;
	src = 0x96;
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 2);
	assert_int_equal(self->P & 0x80,0x80);
	assert_int_equal(self->P & 0x02,0x00);
	assert_int_equal(self->A,0xFF); /* 0x69 | 0x96 = 0xFF */
	/* Result is unsigned and non zero */
	self->P = 0;
	self->A = 0x1A;
	src = 0x4F;
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 2);
	assert_int_equal(self->P & 0x80,0x00);
	assert_int_equal(self->P & 0x02,0x00);
	assert_int_equal(self->A,0x5F); /* 0x1A | 0x4F = 0x5F */
	/* Result is zero */
	self->P = 0;
	self->A = 0x00;
	src = 0x00;
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 2);
	assert_int_equal(self->P & 0x80,0x00);
	assert_int_equal(self->P & 0x02,0x02);
	assert_int_equal(self->A,0x00); /* 0x00 | 0x00 = 0x00 */

	/* Test addressing mode clock */
	inst.opcode = Opcode_Get(0x05); /* ORA ZER */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 3);

	inst.opcode = Opcode_Get(0x15); /* ORA ZEX */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 4);

	inst.opcode = Opcode_Get(0x0D); /* ORA ABS */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 4);

	inst.pageCrossed = 0;
	inst.opcode = Opcode_Get(0x1D); /* ORA ABX */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 4);

	inst.pageCrossed = 1;
	inst.opcode = Opcode_Get(0x1D); /* ORA ABX */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 5);

	inst.pageCrossed = 0;
	inst.opcode = Opcode_Get(0x19); /* ORA ABY */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 4);

	inst.pageCrossed = 1;
	inst.opcode = Opcode_Get(0x19); /* ORA ABY */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 5);

	inst.pageCrossed = 0;
	inst.opcode = Opcode_Get(0x01); /* ORA INX */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 6);

	inst.opcode = Opcode_Get(0x11); /* ORA INY */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 5);
}

static void test_ROL(void **state){
	CPU *self = (CPU*) *state;
	Instruction inst;
	uint8_t (*ptr)(CPU*, Instruction*) = _ROL;
	uint8_t src = 0x00, clk = 0;
	inst.dataMem = &src;

	/* Verify opcode LUT */
	inst.opcode = Opcode_Get(0x2A); /* ROL ACC */
	assert_ptr_equal(ptr, inst.opcode.inst);
	/* Test ROL general behavior */
	/* initial Carry is 0, bit 7 is 0, bit 6 is 0 */
	self->P = 0x00;
	src = 0x3F;
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 2);
	assert_int_equal(self->P & 0x01,0x00);
	assert_int_equal(self->P & 0x80,0x00);
	assert_int_equal(self->P & 0x02,0x00);
	assert_int_equal(src,0x7E); /* 0011 1111 -> 0111 1110 */
	/* initial Carry is 0, bit 7 is 0, bit 6 is 1 */
	self->P = 0x00;
	src = 0x7F;
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 2);
	assert_int_equal(self->P & 0x01,0x00);
	assert_int_equal(self->P & 0x80,0x80);
	assert_int_equal(self->P & 0x02,0x00);
	assert_int_equal(src,0xFE); /* 0111 1111 -> 1111 1110 */
	/* initial Carry is 0, bit 7 is 1, bit 6 is 0 */
	self->P = 0x00;
	src = 0xBF;
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 2);
	assert_int_equal(self->P & 0x01,0x01);
	assert_int_equal(self->P & 0x80,0x00);
	assert_int_equal(self->P & 0x02,0x00);
	assert_int_equal(src,0x7E); /* 1011 1111 -> 0111 1110 */
	/* initial Carry is 0, bit 7 is 1, bit 6 is 1 */
	self->P = 0x00;
	src = 0xFF;
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 2);
	assert_int_equal(self->P & 0x01,0x01);
	assert_int_equal(self->P & 0x80,0x80);
	assert_int_equal(self->P & 0x02,0x00);
	assert_int_equal(src,0xFE); /* 1111 1111 -> 1111 1110 */
	/* initial Carry is 1, bit 7 is 0, bit 6 is 0 */
	self->P = 0x01;
	src = 0x3F;
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 2);
	assert_int_equal(self->P & 0x01,0x00);
	assert_int_equal(self->P & 0x80,0x00);
	assert_int_equal(self->P & 0x02,0x00);
	assert_int_equal(src,0x7F); /* 0011 1111 -> 0111 1111 */
	/* initial Carry is 1, bit 7 is 0, bit 6 is 1 */
	self->P = 0x01;
	src = 0x7F;
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 2);
	assert_int_equal(self->P & 0x01,0x00);
	assert_int_equal(self->P & 0x80,0x80);
	assert_int_equal(self->P & 0x02,0x00);
	assert_int_equal(src,0xFF); /* 0111 1111 -> 1111 1111 */
	/* initial Carry is 1, bit 7 is 1, bit 6 is 0 */
	self->P = 0x01;
	src = 0xBF;
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 2);
	assert_int_equal(self->P & 0x01,0x01);
	assert_int_equal(self->P & 0x80,0x00);
	assert_int_equal(self->P & 0x02,0x00);
	assert_int_equal(src,0x7F); /* 1011 1111 -> 0111 1111 */
	/* initial Carry is 1, bit 7 is 1, bit 6 is 1 */
	self->P = 0x01;
	src = 0xFF;
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 2);
	assert_int_equal(self->P & 0x01,0x01);
	assert_int_equal(self->P & 0x80,0x80);
	assert_int_equal(self->P & 0x02,0x00);
	assert_int_equal(src,0xFF); /* 1111 1111 -> 1111 1111 */
	/* result becomes 0x00 */
	self->P = 0x00;
	src = 0x80;
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 2);
	assert_int_equal(self->P & 0x01,0x01);
	assert_int_equal(self->P & 0x80,0x00);
	assert_int_equal(self->P & 0x02,0x02);
	assert_int_equal(src,0x00); /* 1000 0000 -> 0000 0000 */

	/* Test addressing mode clock */
	inst.opcode = Opcode_Get(0x26); /* ROL ZER */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 5);

	inst.opcode = Opcode_Get(0x36); /* ROL ZEX */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 6);

	inst.opcode = Opcode_Get(0x2E); /* ROL ABS */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 6);

	inst.opcode = Opcode_Get(0x3E); /* ROL ABX */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 7);
}

static void test_ROR(void **state){
	CPU *self = (CPU*) *state;
	Instruction inst;
	uint8_t (*ptr)(CPU*, Instruction*) = _ROR;
	uint8_t src = 0x00, clk = 0;
	inst.dataMem = &src;

	/* Verify opcode LUT */
	inst.opcode = Opcode_Get(0x6A); /* ROR ACC */
	assert_ptr_equal(ptr, inst.opcode.inst);
	/* Test ROL general behavior */
	/* initial Carry is 0, bit 0 is 0 */
	self->P = 0x00;
	src = 0xFE;
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 2);
	assert_int_equal(self->P & 0x01,0x00);
	assert_int_equal(self->P & 0x80,0x00);
	assert_int_equal(self->P & 0x02,0x00);
	assert_int_equal(src,0x7F); /* 1111 1110 -> 0111 1111 */
	/* initial Carry is 0, bit 0 is 1 */
	self->P = 0x00;
	src = 0xFF;
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 2);
	assert_int_equal(self->P & 0x01,0x01);
	assert_int_equal(self->P & 0x80,0x00);
	assert_int_equal(self->P & 0x02,0x00);
	assert_int_equal(src,0x7F); /* 1111 1110 -> 0111 1111 */
	/* initial Carry is 1, bit 0 is 0 */
	self->P = 0x01;
	src = 0xFE;
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 2);
	assert_int_equal(self->P & 0x01,0x00);
	assert_int_equal(self->P & 0x80,0x80);
	assert_int_equal(self->P & 0x02,0x00);
	assert_int_equal(src,0xFF); /* 1111 1110 -> 1111 1111 */
	/* initial Carry is 1, bit 0 is 1 */
	self->P = 0x01;
	src = 0xFF;
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 2);
	assert_int_equal(self->P & 0x01,0x01);
	assert_int_equal(self->P & 0x80,0x80);
	assert_int_equal(self->P & 0x02,0x00);
	assert_int_equal(src,0xFF); /* 1111 1110 -> 1111 1111 */
	/* result becomes 0x00 */
	self->P = 0x00;
	src = 0x01;
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 2);
	assert_int_equal(self->P & 0x01,0x01);
	assert_int_equal(self->P & 0x80,0x00);
	assert_int_equal(self->P & 0x02,0x02);
	assert_int_equal(src,0x00); /* 0000 0001 -> 0000 0000 */

	/* Test addressing mode clock */
	inst.opcode = Opcode_Get(0x66); /* ROR ZER */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 5);

	inst.opcode = Opcode_Get(0x76); /* ROR ZEX */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 6);

	inst.opcode = Opcode_Get(0x6E); /* ROR ABS */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 6);

	inst.opcode = Opcode_Get(0x7E); /* ROR ABX */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 7);
}


static void test_SBC(void **state) {
	CPU *self = (CPU*) *state;
	Instruction inst;
	uint8_t (*ptr)(CPU*, Instruction*)  = _SBC;
	uint8_t src = 0x00, clk = 0;
	inst.dataMem = &src;
	inst.pageCrossed = 0;

	/* Verify Opcode LUT */
	inst.opcode = Opcode_Get(0xE9); /* SBC IMM */
	assert_ptr_equal(ptr, inst.opcode.inst);

	/* Test SBC general behavior */
	/* Signed result, without initial carry */
	// self->P = 0;
	// self->A = 0x01;
	// src = 0x02;
	// clk = inst.opcode.inst(self, &inst);
	// printf("A=%x\n",self->A);
	// assert_int_equal(clk, 2);
	// assert_int_equal(self->P & 0x80, 0x80); /* Sign */
	// assert_int_equal(self->P & 0x02, 0x00); /* Zero */
	// assert_int_equal(self->P & 0x40, 0x00); /* Ovf */
	// assert_int_equal(self->P & 0x01, 0x00); /* Carry out */
	// assert_int_equal(self->A, 0xFF); /* 0x01 - 0x02 = 0xFF (and sign = 1) */
	// /* Signed result, with initial carry */
	// self->P = 0x01;
	// self->A = 0x01;
	// src = 0x02;
	// clk = inst.opcode.inst(self, &inst);
	// printf("A=%x\n",self->A);
	// assert_int_equal(clk, 2);
	// assert_int_equal(self->P & 0x80, 0x80); /* Sign */
	// assert_int_equal(self->P & 0x02, 0x00); /* Zero */
	// assert_int_equal(self->P & 0x40, 0x00); /* Ovf */
	// assert_int_equal(self->P & 0x01, 0x00); /* Carry out */
	// assert_int_equal(self->A, 0xFF); /* 0x01 - 0x02 = 0xFF (and sign = 1) */
	// /* result is zero, which carries out */
	// self->P = 0;
	// self->A = 0x0F;
	// src = 0x0F;
	// clk = inst.opcode.inst(self, &inst);
	// printf("A=%x\n",self->A);
	// assert_int_equal(clk, 2);
	// assert_int_equal(self->P & 0x80, 0x00); /* Sign */
	// assert_int_equal(self->P & 0x02, 0x02); /* Zero */
	// assert_int_equal(self->P & 0x40, 0x00); /* Ovf */
	// assert_int_equal(self->P & 0x01, 0x01); /* Carry out */
	// assert_int_equal(self->A, 0x00); /* 0x0F - 0x0F = 0x00 */

	/* Test addressing mode clock */
	inst.opcode = Opcode_Get(0xE5); /* SBC ZER */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 3);

	inst.opcode = Opcode_Get(0xF5); /* SBC ZEX */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 4);

	inst.opcode = Opcode_Get(0xED); /* SBC ABS */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 4);

	inst.pageCrossed = 0;
	inst.opcode = Opcode_Get(0xFD); /* SBC ABX */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 4);

	inst.pageCrossed = 1;
	inst.opcode = Opcode_Get(0xFD); /* SBC ABX */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 5);

	inst.pageCrossed = 0;
	inst.opcode = Opcode_Get(0xF9); /* SBC ABY */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 4);

	inst.pageCrossed = 1;
	inst.opcode = Opcode_Get(0xF9); /* SBC ABY */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 5);

	inst.pageCrossed = 0;
	inst.opcode = Opcode_Get(0xE1); /* SBC INX */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 6);

	inst.opcode = Opcode_Get(0xF1); /* SBC INY */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 5);

	inst.pageCrossed = 1;
	inst.opcode = Opcode_Get(0xF1); /* SBC INY */
	assert_ptr_equal(ptr, inst.opcode.inst);
	clk = inst.opcode.inst(self, &inst);
	assert_int_equal(clk, 6);
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

static void test_TXA(void **state) {
	CPU* self = (CPU*) *state;
	Instruction inst;
	uint8_t (*ptr)(CPU*, Instruction*)  = _TXA;
	uint8_t clock = 0;

	/* Verify Opcode */
	inst.opcode = Opcode_Get(0x8A); /* TXA */
	assert_ptr_equal(ptr, inst.opcode.inst);

	/* Test TXA behaviour */

	/* POSITIVE VALUE */
	self->X = 0x45;

	clock = inst.opcode.inst(self,&inst);

	assert_int_equal(clock, 2);
	assert_int_equal(self->A, 0X45);

	/* N flag clear */
	uint8_t sr = _GET_SR(self);
	assert_int_equal((sr >> 7) & 1UL, 0);

	/* Z flag clear */
	assert_int_equal((sr >> 1) & 1UL, 0);


	/* NEGATIVE VALUE */
	self->X = 0xF3;

	clock = inst.opcode.inst(self,&inst);

	assert_int_equal(clock, 2);
	assert_int_equal(self->A, 0XF3);

	/* N flag set */
	sr = _GET_SR(self);
	assert_int_equal((sr >> 7) & 1UL, 1);

	/* Z flag clear */
	assert_int_equal((sr >> 1) & 1UL, 0);


	/* ZERO VALUE */
	self->X = 0;

	clock = inst.opcode.inst(self,&inst);

	assert_int_equal(clock, 2);
	assert_int_equal(self->A, 0x0);

	/* N flag clear */
	sr = _GET_SR(self);
	assert_int_equal((sr >> 7) & 1UL, 0);

	/* Z flag set */
	assert_int_equal((sr >> 1) & 1UL, 1);

}

static void test_TXS(void **state) {
	CPU* self = (CPU*) *state;
	Instruction inst;
	uint8_t (*ptr)(CPU*, Instruction*)  = _TXS;
	uint8_t clock = 0;

	/* Verify Opcode */
	inst.opcode = Opcode_Get(0x9A); /* TXS */
	assert_ptr_equal(ptr, inst.opcode.inst);

	/* Test TXS behaviour */

	self->X = 0x93;

	clock = inst.opcode.inst(self,&inst);

	assert_int_equal(clock, 2);
	assert_int_equal(self->SP, 0X93);

}

static void test_TYA(void **state) {
	CPU* self = (CPU*) *state;
	Instruction inst;
	uint8_t (*ptr)(CPU*, Instruction*)  = _TYA;
	uint8_t clock = 0;

	/* Verify Opcode */
	inst.opcode = Opcode_Get(0x98); /* TYA */
	assert_ptr_equal(ptr, inst.opcode.inst);

	/* Test TYA behaviour */

	/* POSITIVE VALUE */
	self->Y = 0x45;

	clock = inst.opcode.inst(self,&inst);

	assert_int_equal(clock, 2);
	assert_int_equal(self->A, 0X45);

	/* N flag clear */
	uint8_t sr = _GET_SR(self);
	assert_int_equal((sr >> 7) & 1UL, 0);

	/* Z flag clear */
	assert_int_equal((sr >> 1) & 1UL, 0);


	/* NEGATIVE VALUE */
	self->Y = 0xF3;

	clock = inst.opcode.inst(self,&inst);

	assert_int_equal(clock, 2);
	assert_int_equal(self->A, 0XF3);

	/* N flag set */
	sr = _GET_SR(self);
	assert_int_equal((sr >> 7) & 1UL, 1);

	/* Z flag clear */
	assert_int_equal((sr >> 1) & 1UL, 0);


	/* ZERO VALUE */
	self->Y = 0;

	clock = inst.opcode.inst(self,&inst);

	assert_int_equal(clock, 2);
	assert_int_equal(self->A, 0x0);

	/* N flag clear */
	sr = _GET_SR(self);
	assert_int_equal((sr >> 7) & 1UL, 0);

	/* Z flag set */
	assert_int_equal((sr >> 1) & 1UL, 1);

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
		cmocka_unit_test(test_AND),
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
		cmocka_unit_test(test_PHA),
		cmocka_unit_test(test_PHP),
		cmocka_unit_test(test_PLA),
		cmocka_unit_test(test_PLP),
		cmocka_unit_test(test_RTI),
		cmocka_unit_test(test_RTS),
		cmocka_unit_test(test_SEI),
		cmocka_unit_test(test_TAX),
		cmocka_unit_test(test_TAY),
		cmocka_unit_test(test_TSX),
		cmocka_unit_test(test_TXA),
		cmocka_unit_test(test_TXS),
		cmocka_unit_test(test_TYA),
		cmocka_unit_test(test_SEC),
		cmocka_unit_test(test_SED),
		cmocka_unit_test(test_STA),
		cmocka_unit_test(test_STX),
		cmocka_unit_test(test_STY),
		cmocka_unit_test(test_ORA),
		cmocka_unit_test(test_ROL),
		cmocka_unit_test(test_ROR),
		cmocka_unit_test(test_SBC)
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
		cmocka_unit_test(test_Instruction_PrintLog),
	};
	int out = 0;
	out += cmocka_run_group_tests(test_instruction_macro, setup_CPU, teardown_CPU);
	out += cmocka_run_group_tests(test_instruction, setup_CPU, teardown_CPU);
	out += cmocka_run_group_tests(test_addressing_Mode, setup_CPU, teardown_CPU);
	out += cmocka_run_group_tests(test_fetch, setup_CPU, teardown_CPU);
	return out;
}

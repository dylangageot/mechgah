#include <stdlib.h>
#include <stdio.h>
#include "ioreg.h"
#include "mapper.h"
#include "../../common/macro.h"

IOReg* IOReg_Create(void) {
	IOReg *self = (IOReg*) malloc(sizeof(IOReg));
	
	/*	If allocation failed, return NULL */
	if (self == NULL) {
		ERROR_MSG("can't allocate IOReg structure");
		return self;
	}

	/* Initialize arrays */
	uint8_t i;
	for (i = 0; i < 40; i++)
		self->acknowledge[i] = AC_NO;
	for (i = 0; i < 8; i++)
		self->bank1[i] = &(self->dummy);
	for (i = 0; i < 32; i++)
		self->bank2[i] = &(self->dummy);

	return self;
}

uint8_t IOReg_Connect(IOReg *self, CPU *cpu, PPU *ppu, Controller *ctrl) {
	if ((self == NULL) || (cpu == NULL) || (ppu == NULL))
		return EXIT_FAILURE;
	
	/* Bank 1 connection */
	self->bank1[PPUCTRL]	= &(ppu->PPUCTRL);
	self->bank1[PPUMASK]	= &(ppu->PPUMASK);
	self->bank1[PPUSTATUS]	= &(ppu->PPUSTATUS);
	self->bank1[OAMADDR]	= &(ppu->OAMADDR);
	self->bank1[OAMDATA]	= &(ppu->OAMDATA);
	self->bank1[PPUSCROLL]	= &(ppu->PPUSCROLL);
	self->bank1[PPUADDR]	= &(ppu->PPUADDR);
	self->bank1[PPUDATA]	= &(ppu->PPUDATA);

	/* Bank 2 connection */
	self->bank2[OAMDMA]		= &(cpu->OAMDMA);
	self->bank2[JOY1]		= &(ctrl->JOY1);
	self->bank2[JOY2]		= &(ctrl->JOY2);

	return EXIT_SUCCESS;
}

uint8_t* IOReg_Get(IOReg *self, uint8_t accessType, uint16_t address) {
	if (self == NULL)
		return NULL;

	/* If address is in 0x2000-0x3FFF */
	if (VALUE_IN(address, 0x2000, 0x3FFF)) {
		self->acknowledge[address & 0x0007] = accessType;
		return self->bank1[address % 8];
	/* If address is in 0x4000-0x4019 */
	} else if (VALUE_IN(address, 0x4000, 0x401F)) {
		self->acknowledge[(address & 0x001F) + 8] = accessType;
		return self->bank2[address & 0x001F];
	}
	
	return NULL;
}

uint8_t IOReg_Ack(IOReg *self, uint16_t address) {
	if (self == NULL)
		return 0;

	uint8_t result = 0;
	/* If address is in 0x2000-0x3FFF */
	if (VALUE_IN(address, 0x2000, 0x3FFF)) {
		result = self->acknowledge[address % 8];
		self->acknowledge[address % 8] = AC_NO;
	/* If address is in 0x4000-0x4019 */
	} else if (VALUE_IN(address, 0x4000, 0x401F)) {
		result = self->acknowledge[(address & 0x001F) + 8];
		self->acknowledge[(address & 0x001F) + 8] = AC_NO;
	}
	
	return result;
}

void IOReg_Destroy(IOReg* self) {
	if (self != NULL)
		free(self);
}

IOReg* IOReg_Extract(Mapper *mapper) {
	/* Get IOReg from mapper */
	return (IOReg*) Mapper_Get(mapper, AS_LDR, LDR_IOR);
}


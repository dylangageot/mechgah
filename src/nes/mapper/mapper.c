#include "mapper.h"
#include <stdlib.h>
#include <stdio.h>

IOReg* IOReg_Create(void) {
	IOReg *self = (IOReg*) malloc(sizeof(IOReg));
	
	/*	If allocation failed, return NULL */
	if (self == NULL) {
		fprintf(stderr, "Error: can't allocate IOReg structure "
				"at %s, line %d.\n", __FILE__, __LINE__);
		return self;
	}

	/* Initialize acknowledge array */
	uint8_t i;
	for (i = 0; i < 40; i++)
		self->acknowledge[i] = 0;

	return self;
}

uint8_t* IOReg_Get(IOReg *self, uint16_t address) {
	if (self == NULL)
		return NULL;

	/* If address is in 0x2000-0x3FFF */
	if (_ADDRESS_IN(address, 0x2000, 0x3FFF)) {
		self->acknowledge[address & 0x0007] = 1;
		return self->bank1 + (address % 8);
	/* If address is in 0x4000-0x4019 */
	} else if (_ADDRESS_IN(address, 0x4000, 0x401F)) {
		self->acknowledge[(address & 0x001F) + 8] = 1;
		return self->bank2 + (address & 0x001F);
	}
	
	return NULL;
}

uint8_t IOReg_Ack(IOReg *self, uint16_t address) {
	if (self == NULL)
		return 0;

	uint8_t result = 0;
	/* If address is in 0x2000-0x3FFF */
	if (_ADDRESS_IN(address, 0x2000, 0x3FFF)) {
		result = self->acknowledge[address % 8];
		self->acknowledge[address % 8] = 0;
	/* If address is in 0x4000-0x4019 */
	} else if (_ADDRESS_IN(address, 0x4000, 0x401F)) {
		result = self->acknowledge[(address & 0x001F) + 8];
		self->acknowledge[(address & 0x001F) + 8] = 0;
	}
	
	return result;
}

void IOReg_Destroy(IOReg* self) {
	if (self != NULL)
		free(self);
}

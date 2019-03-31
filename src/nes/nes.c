#include "nes.h"
#include "loader/loader.h"

NES* NES_Create(char *filename) {
	NES *self = (NES*) malloc(sizeof(NES));
	if (self != NULL) {
		/* Load data from .nes */
		self->mapper = loadROM(filename);
		/* Create instance of CPU */
		self->cpu = CPU_Create(self->mapper);
		/* If an allocation goes wrong, free everything */
		if ((self->mapper == NULL) || (self->cpu == NULL)) {
			fprintf(stderr, "Error: can't allocate memory for NES structure "
				"at %s, line %d.\n", __FILE__, __LINE__);
			NES_Destroy(self);
			return NULL;
		}
		/* Init CPU */
		CPU_Init(self->cpu);
		/* Set to zero clock counter */
		self->clockCount = 0;
	}

	return self;
}

void NES_Destroy(NES *self) {
	if (self == NULL)
		return;
	
	CPU_Destroy(self->cpu);
	if (self->mapper != NULL) {
		/* Free mapper data */
		Mapper_Destroy(self->mapper);
	}
	free(self);
}

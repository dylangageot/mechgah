#include "nes.h"
#include "loader/loader.h"
#include "mapper/ioreg.h"
#include "../common/macro.h"

NES* NES_Create(char *filename) {
	NES *self = (NES*) malloc(sizeof(NES));
	if (self != NULL) {
		/* Load data from .nes */
		self->mapper = loadROM(filename);
		/* Create instance of CPU */
		self->cpu = CPU_Create(self->mapper);
		/* Create instance of PPU */
		self->ppu = PPU_Create(self->mapper);
		/* Create instance of Controller */
		self->controller = Controller_Create(self->mapper);
		/* If an allocation goes wrong, free everything */
		if ((self->mapper == NULL) || (self->cpu == NULL) ||
			(self->ppu == NULL) || (self->controller == NULL)) {
			ERROR_MSG("can't allocate memory for NES");
			NES_Destroy(self);
			return NULL;
		}
		/* Connect component together */
		IOReg_Connect(IOReg_Extract(self->mapper), self->cpu, self->ppu, self->controller);
		/* Init CPU */
		CPU_Init(self->cpu);
		/* Init PPU */
		PPU_Init(self->ppu);
		/* Set to zero clock counter */
		self->clockCount = 0;
		/* Set to RESET context */
		self->context = 0x01;

	} else
		ERROR_MSG("can't allocate NES structure");

	return self;
}

uint8_t NES_NextFrame(NES *self, uint16_t keysPressed) {
	uint32_t previousClockCount = self->clockCount;
	while (PPU_PictureDrawn(self->ppu) == 0) {
		if (CPU_Execute(self->cpu, &self->context, &self->clockCount) 
				== EXIT_FAILURE)
			return EXIT_FAILURE;
		PPU_Execute(self->ppu, &self->context, 
				(self->clockCount - previousClockCount) * 3);
		Controller_Execute(self->controller, keysPressed);
		previousClockCount = self->clockCount;
	}
	return EXIT_SUCCESS;
}

uint32_t* NES_Render(NES *self) {
	if (self == NULL)
		return NULL;
	if (self->ppu == NULL)
		return NULL;
	/* Return pixel array */
	return self->ppu->image;
}

void NES_Destroy(NES *self) {
	if (self == NULL)
		return;
	CPU_Destroy(self->cpu);
	PPU_Destroy(self->ppu);
	Controller_Destroy(self->controller);
	if (self->mapper != NULL) {
		/* Free mapper data */
		Mapper_Destroy(self->mapper);
	}
	free(self);
}

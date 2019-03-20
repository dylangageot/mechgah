#include "src/nes/nes.h"

int main(int argc, char **argv) {
	if (argc == 1) {
		fprintf(stderr, "Please specify a .nes image path\n");
		return EXIT_FAILURE;
	}

	NES *nes = NES_Create(argv[1]);
	if (nes == NULL)
		return EXIT_FAILURE;

	int i;
	uint8_t context = 1;
	/* If loading nestest.nes, replace reset vector to automate test */
	*(nes->mapper->get(nes->mapper->memoryMap, AS_CPU, 0xFFFC)) = 0x00;
	nes->cpu->P |= 0x20;

	for (i = 0; i < 256; i++) {
		CPU_Execute(nes->cpu, &context, &nes->clockCount);
	}

	fprintf(stdout, "Number of clock cycle consumed: %d\n", nes->clockCount);

	NES_Destroy(nes);
	return EXIT_SUCCESS;
}

/*
 * Mechgah, a precise NES emulator.
 * Developped by Nicolas Chabanis, Nicolas Hily, Baptiste Mehat and
 * Dylan Gageot, student at INSA Rennes.
 */

#include "src/app.h"

int main(int argc, char **argv) {
	/* Instanciate App structure */
	App app;

	/* Init application */
	if (App_Init(&app, argc, argv) == EXIT_FAILURE)
		return EXIT_FAILURE;

	/* Execute application */
	return App_Execute(&app);
}

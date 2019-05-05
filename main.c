/**
 * \file main.c
 * \brief Main source of NES Emulator
 * \author Dylan Gageot
 * \version 1.0
 * \date 2019-05-05
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

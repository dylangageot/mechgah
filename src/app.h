/**
 * \file app.h
 * \brief NES Emulator application based on SDL
 * \author Dylan Gageot
 * \version 1.0
 * \date 2019-05-05
 */

#ifndef APP_H
#define APP_H

#include <SDL/SDL.h>
#include "nes/nes.h"

#define TICK_INTERVAL 16

typedef struct {
	/* Emulator */
	NES *nes;
	/* SDL */
	SDL_Surface *screen;
	uint16_t keysConfig[16];
	/* Render and timing information */
	uint8_t scale;
	uint32_t nextFlip;	
} App;

/**
 * \fn App_Init
 * \brief Retrieve information from given launch option and init emulator
 *
 * \param self instance of App
 * \param argc value of argc
 * \param argv address of argv
 * 
 * \return EXIT_SUCCESS if succeed, EXIT_FAILURE otherwise
 */
uint8_t App_Init(App *self, int argc, char **argv);


/**
 * \fn App_TimeLeft
 * \brief Give time left for delaying screen refresh
 *
 * \param self instance of App
 *
 * \return time left
 */
uint32_t App_TimeLeft(App *self);

/**
 * \fn App_Execute
 * \brief Execute application
 *
 * \param self instance of App
 *
 * \return EXIT_SUCCESS if succeed, EXIT_FAILURE otherwise
 */
uint8_t App_Execute(App *self);

#endif /* APP_H */

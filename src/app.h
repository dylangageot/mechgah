/**
 * \file app.h
 * \brief header file of App using SDL
 * \author Dylan Gageot
 * \version 1.0
 * \date 2019-05-05
 */

#ifndef APP_H
#define APP_H

#include <SDL/SDL.h>
#include "nes/nes.h"

#define TICK_INTERVAL 16

/**
 * \brief Hold application data
 */
typedef struct {
	/* Emulator */
	NES *nes;						/*!< Instance of NES emulator	*/
	/* SDL */
	SDL_Surface *screen;			/*!< Main screen surface		*/
	uint16_t keysConfig[16];		/*!< Key configuration			*/
	/* Render and timing information */
	uint8_t scale;					/*!< Scale factor for rendering	*/
	uint32_t nextFlip;				/*!< Timestamp to next frame	*/
} App;

/**
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
 * \brief Give time left for delaying screen refresh
 *
 * \param self instance of App
 *
 * \return time left
 */
uint32_t App_TimeLeft(App *self);

/**
 * \brief Execute application
 *
 * \param self instance of App
 *
 * \return EXIT_SUCCESS if succeed, EXIT_FAILURE otherwise
 */
uint8_t App_Execute(App *self);

#endif /* APP_H */

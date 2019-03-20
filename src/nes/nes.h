/**
 * \file nes.h
 * \brief Header for NES emulator
 * \author Dylan Gageot
 * \version 1.0
 * \date 2019-02-25
 */

#ifndef NES_H
#define NES_H

#include "cpu/cpu.h"
/* #include "ppu/ppu.h" */
#include "mapper/mapper.h"
#include "loader/loader.h"

/**
 * \struct NES
 * \brief Hold every component to emulate the Nitendo Entertainement System
 */
typedef struct {
	CPU *cpu;
	/* PPU ppu; */
	Mapper *mapper;
	uint32_t clockCount;
} NES;

/**
 * \fn NES_Create
 * \brief Allocate memory for the emulator and load the ROM provide in arg.
 *
 * \param filename path that point a .nes file
 *
 * \return instance of NES allocated
 */
NES* NES_Create(char *filename);

/**
 * \fn NES_NextFrame
 * \brief Execute the system for one frame and render it
 *
 * \return pixel map
 */
/* SDL_Surface NES_NextFrame(void); */

/**
 * \brief Update event for the controller
 *
 * \param event event from the keyboard
 */
/* void NES_UpdateEvent(SDL_Event event); */

/**
 * \fn NES_Destroy
 * \brief Free the memory used by the emulator
 *
 * \param self instance of NES
 */
void NES_Destroy(NES *self);

#endif /* NES_H */

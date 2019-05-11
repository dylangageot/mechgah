/**
 * \file nes.h
 * \brief header file of NES module
 * \author Dylan Gageot
 * \version 1.0
 * \date 2019-02-25
 */

#ifndef NES_H
#define NES_H

#include "cpu/cpu.h"
#include "ppu/ppu.h"
#include "mapper/mapper.h"
#include "loader/loader.h"
#include "controller/controller.h"

/**
 * \brief Hold every component to emulate the Nintendo Entertainement System
 */
typedef struct {
	CPU *cpu;
	PPU *ppu;
	Controller *controller;
	Mapper *mapper;
	uint32_t clockCount;
	uint8_t context;
} NES;

/**
 * \brief Allocate memory for the emulator and load the ROM provide in arg.
 *
 * \param filename path that point a .nes file
 *
 * \return instance of NES allocated
 */
NES* NES_Create(char *filename);

/**
 * \brief Execute the system for one frame
 * \param self instance of NES
 * \param keysPressed keys pressed on keyboard
 * \return EXIT_SUCCESS
 */
uint8_t NES_NextFrame(NES *self, uint16_t keysPressed);

/**
 * \brief Render image from PPU
 *
 * \param self instance of NES
 *
 * \return image array with color in 32 bits
 */
uint32_t* NES_Render(NES *self);

/**
 * \brief Free the memory used by the emulator
 *
 * \param self instance of NES
 */
void NES_Destroy(NES *self);

#endif /* NES_H */

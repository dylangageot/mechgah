/**
 * \file loader.h
 * \brief Header for the ROM loader 
 * \author Dylan Gageot
 * \version 1.0
 * \date 2019-02-22
 */

#ifndef LOADER_H
#define LOADER_H

#include "cpu.h"

/**
 * \brief Load ROM into Mapper structure
 *
 * \param filename .nes file to load
 *
 * \return instance of Mapper
 */
Mapper* loadROM(char* filename);

#endif /* LOADER_H */

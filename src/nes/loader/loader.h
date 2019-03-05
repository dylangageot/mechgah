/**
 * \file loader.h
 * \brief Header for the ROM loader
 * \author Nicolas Chabanis
 * \version 1.0
 * \date 2019-02-22
 */

#ifndef LOADER_H
#define LOADER_H

//#include "../cpu/cpu.h"
#include "../mapper/mapper.h"

/**
 * \struct Header
 * \brief Header of a .nes file
 */
 typedef struct{
 	__uint8_t N;
 	__uint8_t E;
 	__uint8_t S;
 	__uint8_t EOF;
  __uint8_t romSize; // Number of 16 kB ROM (PRG-ROM) banks
  __uint8_t vromSize; // Number of 8 kB VROM (CHR-ROM) banks
  __uint8_t flags6; // Mapper, mirroring, battery, trainer
  __uint8_t flags7; // Mapper, VS/Playchoice, NES 2.O
  __uint8_t flags8; // PRG-RAM size
  __uint8_t flags9; // TV System
  __uint32_t flags10to13; // Must be all 0
  __uint16_t flags14to15; // Must be all 0
 } Header;

/**
 * \brief Load ROM into Mapper structure
 * \param filename .nes file to load
 * \return instance of Mapper
 */
Mapper* loadROM(char* filename);

#endif /* LOADER_H */

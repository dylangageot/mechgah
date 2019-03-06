/**
 * \file loader.h
 * \brief Header for the ROM loader
 * \author Nicolas Chabanis
 * \version 1.0
 * \date 2019-02-22
 */

#ifndef LOADER_H
#define LOADER_H

#include <stdint.h>
#include <string.h>
#include "../cpu/cpu.h"
#include "../mapper/mapper.h"
#include "../mapper/nrom.h"

#define MAPPER_TOTAL 1

/**
 * \struct Header
 * \brief Header of a .nes file, modified for programming purposes
 */
typedef struct{
  uint8_t romSize; // Number of 16 kB ROM (PRG-ROM) banks
  uint8_t vromSize; // Number of 8 kB VROM (CHR-ROM) banks
  uint8_t ramSize; // Size of PRG RAM, in 8 kB units
  uint8_t mapper; // iNES 1.0 mapper number
  uint8_t mirroring;
  // 1=Vertical mirroring, 0=Horizontal or mapper-controlled mirroring
  uint8_t battery_backed_RAM;
  // 1=Batter-backed RAM at $6000-$7FFF or other persistent memory
  uint8_t trainer; // 1=512-Byte trainer at $7000-$71FF
  uint8_t four_screen_VRAM; // 1=Hard-wired Four-screen VRAM layout
  uint8_t VS_System; // 1=VS-System cartridges
  uint8_t playchoice; // 1=Playchoice-10 bit, Not official
  //uint8_t NES2; // 1=NES 2.0 format
  uint8_t tvSystem; // 0=PAL, 1=NSTC

} Header;

/**
 * \brief Load ROM into Mapper structure
 * \param filename .nes file to load
 * \return instance of Mapper
 */
Mapper* loadROM(char* filename);

#endif /* LOADER_H */

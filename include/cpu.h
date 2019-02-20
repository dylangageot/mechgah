/**
 * \file cpu.h
 * \brief Header file of the CPU module
 * \author Dylan Gageot
 * \version 1.0
 * \date 2019-02-13
 */

#ifndef CPU_H
#define CPU_H

#include "nrom.h"
#include "instructions.h"

/**
 * \struct CPU
 * \brief Hold CPU's register and memory
 */
typedef struct {
	uint8_t A, X, Y, SP, P;							/*! 8-bit registers */
	uint16_t PC;									/*! 16-bit register */
	uint8_t* (*mapper)(void*, uint8_t, uint16_t);	/*! Mapper callback */
	void* memoryMap;								/*! Memory map		*/
} CPU;

/**
 * \fn CPU_Create
 * \brief Allocate memory for CPU struct
 *
 * \param mapper Mapper callback
 *
 * \return instance of CPU allocated 
 */
CPU* CPU_Create(uint8_t* (*mapper)(void*, uint8_t, uint16_t));

/**
 * \fn CPU_SetMemoryMap
 * \brief Set memory map (mapper data)
 *
 * \param self instance of CPU
 * \param memoryMap instance of memoryMap
 */
void CPU_SetMemoryMap(CPU* self, void* memoryMap);

/**
 * \fn CPU_GetMemoryMap
 * \brief Get memory map (mapper data)
 *
 * \param self instance of CPU
 *
 * \return instance of memoryMap
 */
void* CPU_GetMemoryMap(CPU* self);

/**
 * \fn CPU_Execute
 * \brief Execute the next instruction
 *
 * \param self instance of CPU
 * \param context variable that contain IRQ
 *
 * \return number of clock cycle used to execute the instruction 
 */
uint8_t CPU_Execute(CPU* self, uint8_t context);

/**
 * \fn CPU_Destroy
 * \brief Free CPU's instance
 *
 * \param self
 */
void CPU_Destroy(CPU* self);

#endif /* CPU_H */

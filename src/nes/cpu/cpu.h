/**
 * \file cpu.h
 * \brief Header file of the CPU module
 * \author Dylan Gageot
 * \version 1.0
 * \date 2019-02-13
 */

#ifndef CPU_H
#define CPU_H

#define NMI_JMP_ADD 0xFFFA
#define RES_JMP_ADD 0xFFFC
#define IRQ_JMP_ADD 0xFFFE


#include "../mapper/mapper.h"

/**
 * \struct CPU
 * \brief Hold CPU's register and memory
 */
typedef struct {
	uint8_t A, X, Y, SP, P;					/*! 8-bit registers		    */
	uint16_t PC;							/*! 16-bit register		    */
	Mapper* rmap;							/*! Mapper from NES struct	*/
} CPU;

/**
 * \fn CPU_Create
 * \brief Allocate memory for CPU structure
 *
 * \param mapper address of Mapper pointer from NES struct
 *
 * \return instance of CPU allocated
 */
CPU* CPU_Create(Mapper* mapper);

/**
 * \fn CPU_Init
 * \brief Initialize all CPU registers (A,X,Y,SP,P adn PC)
 *
 * \param self instance of CPU
 *
 * \return 0 if success, 1 otherwise
 */

uint8_t CPU_Init(CPU* self);

/**
 * \fn CPU_InterruptManager
 * \brief Handle the NMI, IRQ and BRK interrupts
 *
 * \param self instance of CPU
 * \param context variable that contains interrupt flags.
 *		xxxx xINR :
 *			- R : RESET signal detected
 *			- N : NMI detected at the end of the previous instruction
 *			- I : IRQ detected at the end of the previous instruction
 *			- x : non used bits
 * \return number of clock cycle used to execute the instruction
*/
uint8_t CPU_InterruptManager(CPU* self, uint8_t* context);

/**
 * \fn CPU_Execute
 * \brief Execute the next instruction
 *
 * \param self instance of CPU
 * \param context variable containing interrupt flags
 * \param clockCycle pointer to clock cycle variable
 *
 * \return number of clock cycle used to execute the instruction
 */
uint32_t CPU_Execute(CPU* self, uint8_t* context, uint32_t *clockCycle);

/**
 * \fn CPU_Destroy
 * \brief Free CPU's instance
 *
 * \param self
 */
void CPU_Destroy(CPU* self);

#endif /* CPU_H */

/**
 * \file cpu.h
 * \brief header file of CPU module
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
 * \brief Hold CPU's register and memory
 */
typedef struct {
	/* IO Register */
	uint8_t OAMDMA;							/*!< CPU address to copy 256 bytes to OAM */
	/* Internal Register */
	uint8_t A;								/*!< Accumulator			*/
	uint8_t X;								/*!< X index				*/
	uint8_t Y;								/*!< Y index				*/
	uint8_t SP;								/*!< Stack Pointer			*/
	uint8_t P;								/*!< Status					*/
	uint16_t PC;							/*!< Program counter		*/
	int16_t cntDMA;							/*!< DMA counter			*/
	Mapper* mapper;							/*!< Mapper to get data from*/
} CPU;

/**
 * \brief Allocate memory for CPU structure
 *
 * \param mapper address of Mapper pointer from NES struct
 *
 * \return instance of CPU allocated
 */
CPU* CPU_Create(Mapper* mapper);

/**
 * \brief Initialize all CPU registers (A, X, Y, SP, P and PC)
 *
 * \param self instance of CPU
 *
 * \return EXIT_SUCCESS if succeed, EXIT_FAILURE otherwise
 */

uint8_t CPU_Init(CPU* self);

/**
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
 * \brief Execute the next instruction
 *
 * \param self instance of CPU
 * \param context variable containing interrupt flags
 * \param clockCycle pointer to clock cycle variable
 *
 * \return number of clock cycle used to execute the instruction
 */
uint8_t CPU_Execute(CPU* self, uint8_t* context, uint32_t *clockCycle);

/**
 * \brief Free CPU's instance
 *
 * \param self
 */
void CPU_Destroy(CPU* self);

#endif /* CPU_H */

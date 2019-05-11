/**
 * \file mapper.h
 * \brief header file of Mapper module
 * \author Dylan Gageot
 * \version 1.0
 * \date 2019-02-20
 */

#ifndef MAPPER_H
#define MAPPER_H

#include "stdint.h"

/**
 * \struct Mapper
 * \brief Generic structure to hold mapper
 */
typedef struct {
	void* (*get)(void*, uint8_t, uint16_t);			/*! Get callback	     */
	void (*destroyer)(void*);						/*! Destroyer callback	 */
	uint8_t (*ack)(void*, uint16_t);				/*! Acknowledge callback */
	void *mapperData;								/*! Mapper data			 */
} Mapper;

/**
 * \brief Create instance of mapper from callback and data of specific mapper
 *
 * \param get Get callback
 * \param destroyer Destroyer callback
 * \param ack Acknowledge callback
 * \param mapperData Mapper data
 *
 * \return instance of Mapper
 */
Mapper* Mapper_Create(void* (*get)(void*, uint8_t, uint16_t),
					  void (*destroyer)(void*),
					  uint8_t (*ack)(void*, uint16_t),
					  void *mapperData);

/**
 * \brief Destroy instance of Mapper
 *
 * \param self free Mapper allocation
 */
void Mapper_Destroy(Mapper *self);

/**
 * \brief Get pointer from memory for a specific address
 *
 * \param self instance of Mapper
 * \param space in which address space to look and with type of access
 * \param address address to get data from
 *
 * \return pointer of pointed data
 */
uint8_t* Mapper_Get(Mapper *self, uint8_t space, uint16_t address);

/**
 * \brief Ask for the last type of access at a register and aknowledge it
 *
 * \param self instance of Mapper
 * \param address address to get access information from
 *
 * \return 1 if address accesed before, 0 otherwise
 */
uint8_t Mapper_Ack(Mapper *self, uint16_t address);

/**
 * \brief Use to specify to the mapper in which address space we want to 
 * retrieve the data.
 */
enum AddressSpace {
	AS_CPU = 0,			/*!< CPU address space		*/
	AS_PPU,				/*!< PPU addrsss space		*/
	AS_LDR				/*!< Loader special access	*/
};

/**
 * \brief Use to get pointer to PRGROM and CHRROM
 */
enum LoaderData {
	LDR_PRG = 0,		/*!< Get pointer for PGR-ROM	*/
	LDR_CHR,			/*!< Get pointer for CHR		*/
	LDR_IOR				/*!< Get pointer for IOReg		*/
};

#endif /* MAPPER_H */

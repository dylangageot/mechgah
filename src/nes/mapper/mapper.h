/**
 * \file mapper.h
 * \brief Header including general structure for mapper
 * \author Dylan Gageot
 * \version 1.0
 * \date 2019-02-20
 */

#ifndef MAPPER_H
#define MAPPER_H

#include "stdint.h"

/**
 * \brief Useful macro when dealing with address mapping
 */
#define _ADDRESS_IN(x,y,z)	(((x) >= (y)) && ((x) <= (z)))
#define _ADDRESS_SUP(x,y)	(((x) >= (y)))
#define _ADDRESS_INF(x,y)	(((x) <= (y)))

/**
 * \struct Mapper
 * \brief Generic structure to hold mapper
 */
typedef struct {
	uint8_t* (*get)(void*, uint8_t, uint16_t);		/*! Get callback	     */
	void (*destroyer)(void*);						/*! Destroyer callback	 */
	uint8_t (*ack)(void*, uint16_t);				/*! Acknowledge callback */
	void *mapperData;								/*! Mapper data			 */
} Mapper;

/**
 * \brief Mapper_Create
 *
 * \param get Get callback
 * \param destroyer Destroyer callback
 * \param ack Acknowledge callback
 * \param mapperData Mapper data
 *
 * \return instance of Mapper
 */
Mapper* Mapper_Create(uint8_t* (*get)(void*, uint8_t, uint16_t),
					  void (*destroyer)(void*),
					  uint8_t (*ack)(void*, uint16_t),
					  void *mapperData);

/**
 * \brief Mapper_Destroy
 *
 * \param self free Mapper allocation
 */
void Mapper_Destroy(Mapper *self);

/**
 * \brief Mapper_Get
 *
 * \param self instance of Mapper
 * \param space in which address space to look ?
 * \param address address to get data from
 *
 * \return pointer of pointed data
 */
uint8_t* Mapper_Get(Mapper *self, uint8_t space, uint16_t address);

/**
 * \brief Mapper_Ack
 *
 * \param self instance of Mapper
 * \param address address to get access information from
 *
 * \return 1 if address accesed before, 0 otherwise
 */
uint8_t Mapper_Ack(Mapper *self, uint16_t address);

/**
 * \enum AddressSpace
 * \brief Use to specify to the mapper in which address space we want to 
 * retrieve the data.
 */
enum AddressSpace {
	AS_CPU = 0,
	AS_PPU,
	AS_LDR
};

/**
 * \enum LoaderData
 * \brief Use to get pointer to PRGROM and CHRROM
 */
enum LoaderData {
	LDR_PRG = 0,
	LDR_CHR
};

#endif /* MAPPER_H */

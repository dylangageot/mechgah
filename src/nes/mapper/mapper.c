#include "mapper.h"
#include <stdlib.h>
#include <stdio.h>

Mapper* Mapper_Create(void* (*get)(void*, uint8_t, uint16_t),
					  void (*destroyer)(void*),
					  uint8_t (*ack)(void*, uint16_t),
					  void *mapperData) {
	Mapper *self = (Mapper*) malloc(sizeof(Mapper));
	if (self == NULL) {
		fprintf(stderr, "Error: can't allocate Mapper structure "
				"at %s, line %d.\n", __FILE__, __LINE__);
		return self;
	}

	/* Init Mapper structure */
	self->get = get;
	self->destroyer = destroyer;
	self->ack = ack;
	self->mapperData = mapperData;
	return self;
}

void Mapper_Destroy(Mapper *self) {
	if (self == NULL)
		return;
	/* If there is a mapper data, destroy it */
	if ((self->mapperData != NULL) && (self->destroyer != NULL))
		self->destroyer(self->mapperData);
	free((void*) self);
}

uint8_t* Mapper_Get(Mapper *self, uint8_t space, uint16_t address) {
	if (self == NULL)
		return NULL;

	/* If there is a mapper data and get's callback, use it */
	if ((self->mapperData != NULL) && (self->get != NULL))
		return (uint8_t*) self->get(self->mapperData, space, address);
	else
		return NULL;
}

uint8_t Mapper_Ack(Mapper *self, uint16_t address) {
	if (self == NULL)
		return 0;

	/* If there is a mapper data and ack's callback, use it */
	if ((self->mapperData != NULL) && (self->ack != NULL))
		return self->ack(self->mapperData, address);
	else
		return 0;
}

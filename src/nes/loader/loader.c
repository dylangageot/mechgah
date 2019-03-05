/**
 * \file loader.c
 * \brief ROM loader file
 * \author Nicolas CHABANIS
 * \version 1.0
 * \date 2019-03-2
 */

#include "loader.h"

/* Load ROM into Mapper structure */
Mapper * loadROM(char* filename){
	/* Opening the file whose name is given in parameters */
  FILE *romFile = NULL;
  romFile = fopen(filename,"rb");
  if(romFile ==  (FILE*)NULL){
    fprintf(stderr,"Couldn't open ROM file in loadROM function.\n");
		return (Mapper*)NULL;
  }
	/* Allocating structure memory */
  Mapper * mapper = malloc(sizeof(Mapper));
  if(mapper == (Mapper*)NULL){
    fprintf(stderr, "Couldn't allocate Mapper structure memory in loadROM function.\n");
    return (Mapper*)NULL;
  }
  Header * header = malloc(sizeof(Header));
	if(header == (Header*)NULL){
    fprintf(stderr, "Couldn't allocate Header structure memory in loadROM function.\n");
    return (Mapper*)NULL;
  }
	/* Storing the .nes header into the Header structure */
  if (fread(h,sizeof(Header),1,romFile) != 1){
		pfrintf(stderr, "Error while reading the file header in loadROM function\n");
		return (Mapper*)NULL;
	}
	/* Checking the file format */
	if(h->N!='N' || h->E!='E' || h->S!='S' || h->EOF!=26){
		pfrintf(stderr, "Given ROM is not a .nes file\n");
		return (Mapper*)NULL;
	}
	/* Checking its integrity */
	if(h->flags10to13!=0 || h->flags14to15!=0){
		pfrintf(stderr,"Given ROM is not upright or may be a rip\n");
		return (Mapper*)NULL;
	}
  

	/* Returning the mapper */
	return mapper;
}

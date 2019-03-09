/**
 * \file loader.c
 * \brief ROM loader file
 * \author Nicolas CHABANIS
 * \version 1.0
 * \date 2019-03-2
 */

#include "loader.h"

/* Map<Mapper>_Create function LUT */
void* createLUT[MAPPER_TOTAL] = {
  MapNROM_Create,
};

/* Map<Mapper>_Get function LUT */
uint8_t* getLUT[MAPPER_TOTAL] = {
  MapNROM_Get,
};

/* Map<Mapper>_Destroy function LUT */
void* destroyLUT[MAPPER_TOTAL] = {
  MapNROM_Destroy,
};

/* Fills the Header structure */
void fillHeader(Header * header, char * h){
  header->romSize = h[4];
  header->vromSize = h[5];
  header->ramSize = h[8];
  header->mapper = (h[7] & 0xF0) | (h[6] & 0xF0);
  header->mirroring = (h[6] & 0x01);
  header->battery_backed_RAM = (h[6] & 0x02)>0;
  header->trainer = (h[6] & 0x04)>0;
  header->four_screen_VRAM = (h[6] & 0x08)>0;
  header->VS_System = (h[7] & 0x01)>0;
  header->playchoice = (h[7] & 0x02)>0;
  //header->NES2 = (h[7] & 0x0C)==16;
  header->tvSystem = (h[9] & 0x01);
}

/* Load ROM into Mapper structure */
Mapper * loadROM(char* filename){

	/* Opening the file whose name is given in parameters */
  FILE *romFile = NULL;
  romFile = fopen(filename,"rb");
  if(romFile ==  (FILE*)NULL){
    fprintf(stderr,"Couldn't open ROM file \
                  at %s, line %d\n", __FILE__, __LINE__);
		return (Mapper*)NULL;
  }

	/* Allocating structure memory */
  Mapper * mapper = malloc(sizeof(Mapper));
  if(mapper == (Mapper*)NULL){
    fprintf(stderr, "Couldn't allocate Mapper structure memory \
                    at %s, line %d\n", __FILE__, __LINE__);
    return (Mapper*)NULL;
  }
  Header * header = malloc(sizeof(Header));
	if(header == (Header*)NULL){
    fprintf(stderr, "Couldn't allocate Header structure memory \
                    at %s, line %d\n", __FILE__, __LINE__);
    return (Mapper*)NULL;
  }

  /* Storing the .nes header into a 16 Byte table */
  char h[16];
  if (fread(h,16,1,romFile) != 1){
		fprintf(stderr, "Error while reading the file header \
                    at %s, line %d\n", __FILE__, __LINE__);
		return (Mapper*)NULL;
	}

  /* Checking the file format */
  if(h[0]!='N' || h[1]!='E' || h[2]!='S' || h[3]!=26){
    fprintf(stderr, "Given ROM is not a .nes file\n");
    return (Mapper*)NULL;
  }

  /* Checking its integrity, only supports iNES (1.0) format */
  for(int i=10; i<16 ; i++){
    if(h[i]!=0){
      fprintf(stderr,"Given ROM is not upright or may be a rip\n");
      return (Mapper*)NULL;
    }
  }

  /* Filling the Header structure */
  fillHeader(header,h);

  /* Checking if mapper is described */
  if(header->mapper > MAPPER_TOTAL){
    fprintf(stderr,"ROM Mapper is not described (yet)");
    return (Mapper*)NULL;
  }

  /* Creating the needed mapper */
  createMapper = createLUT[header->mapper];
  void * memoryMap = (*createMapper)(header);

  /* Filling the Mapper structure */
  mapper->get = getLUT[header->mapper];
  mapper->destroyer = destroyLUT[header->mapper];
  mapper->memoryMap = memoryMap;


  /* Fetching the ROM and VROM adresses */
  /*
  uint8_t * romAddr = mapper->get(mapper->memoryMap,AS_LDR,LDR_PRG);
  uint8_t * vromAddr = mapper->get(mapper->memoryMap,AS_LDR,LDR_CHR);
  */

  /* Copying the programm into the Mapper structure*/
  /*
  memcpy(romAddr,romFile+16+(header->trainer)*64,mapper->romSize);
  memcpy(vromAddr,romFile+16+(header->trainer)*64+(header->romSize)*16000,mapper->vromSize);
  */

	/* Returning the Mapper structure */
	return mapper;
}

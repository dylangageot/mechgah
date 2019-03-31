/**
 * \file loader.c
 * \brief ROM loader file
 * \author Nicolas CHABANIS
 * \version 1.0
 * \date 2019-03-2
 */

#include "loader.h"
#include "../mapper/nrom.h"

/* Mapper function LUT */
Mapper* (*createLUT[MAPPER_TOTAL])(Header*) = {
	MapNROM_Create,		/* 0 NROM/no mapper */
	NULL,				/* 1 MMC1 */
	NULL,				/* 2 CNROM */
	NULL,				/* 3 UNROM */
	NULL,				/* 4 MMC3 */
	NULL,				/* 5 MMC5 */
	NULL,				/* 6 FFE F4xxx */
	NULL,				/* 7 AOROM */
	NULL,				/* 8 FFE F3xxx */
	NULL,				/* 9 MMC2 */
	NULL,				/* 10 MMC4 */
	NULL,				/* 11 ColorDreams chip */
	NULL,				/* 12 FFE F6xxx */
	NULL,				/* 13 CPROM */
	NULL,				/* 14 Unused */
	NULL,				/* 15 100-in-1 */
	NULL,				/* 16 Bandai */
	NULL,				/* 17 FFE F8xxx */
	NULL,				/* 18 Jaleco */
	NULL,				/* 19 Namcot */
	NULL,				/* 20 Nintendo DiskSystem (reserved) */
	NULL,				/* 21 Konami VRC4a */
	NULL,				/* 22 Konami VRC2a */
	NULL,				/* 23 Konami VRC2a */
	NULL,				/* 24 Konami VRC6 */
	NULL,				/* 25 Konami VRC4b */
};

/* Fills the Header structure */
void fillHeader(Header * header, uint8_t * h){
	header->romSize = h[4];
	header->vromSize = h[5];
	header->ramSize = h[8];
	header->mapper = ( (h[7] & 0xF0) | ((h[6] & 0xF0)>>4));
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
		fprintf(stderr,"Couldn't open ROM file (is the path right?) "
				"at %s, line %d\n", __FILE__, __LINE__);
		return NULL;
	}

	Header * header = malloc(sizeof(Header));
	if(header == (Header*)NULL){
		fprintf(stderr, "Couldn't allocate Header structure memory "
				"at %s, line %d\n", __FILE__, __LINE__);
		free(header);
		fclose(romFile);
		return NULL;
	}

	/* Storing the .nes header into a 16 unsigned Byte table */
	uint8_t h[16];
	if (fread(h,16,1,romFile) != 1){
		fprintf(stderr, "Error while reading the file header "
				"at %s, line %d\n", __FILE__, __LINE__);
		free(header);
		fclose(romFile);
		return NULL;
	}

	/* Checking the file format */
	if(h[0]!='N' || h[1]!='E' || h[2]!='S' || h[3]!=26){
		fprintf(stderr, "Given ROM is not a .nes file\n");
		free(header);
		fclose(romFile);
		return NULL;
	}

	/* Checking its integrity, only supports iNES (1.0) format */
	for(int i=10; i<16 ; i++){
		if(h[i]!=0){
			fprintf(stderr,"Given ROM is not upright or may be a rip\n");
			free(header);
			fclose(romFile);
			return NULL;
		}
	}

	/* Filling the Header structure */
	fillHeader(header,h);

	/* Checking if mapper is described */
	if(header->mapper > MAPPER_TOTAL || createLUT[header->mapper] == NULL){
		fprintf(stderr,"ROM Mapper is not described (yet)\n");
		free(header);
		fclose(romFile);
		return NULL;
	}

	/* Creating the needed mapper */
	Mapper *mapper = createLUT[header->mapper](header);

	/* Fetching the ROM and VROM adresses */
	void * romAddr = Mapper_Get(mapper, AS_LDR, LDR_PRG);
	void * vromAddr = Mapper_Get(mapper, AS_LDR, LDR_CHR);

	/* Copying the programm into the Mapper structure*/
	fseek(romFile,16,SEEK_SET);
	fread(romAddr,(header->romSize)*16384,1,romFile);
	fseek(romFile,16+(header->romSize)*16384,SEEK_SET);
	fread(vromAddr,(header->vromSize)*8192,1,romFile);

	/* Returning the Mapper structure */
	free(header);
	fclose(romFile);
	return mapper;
}

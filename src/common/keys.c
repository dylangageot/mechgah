#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include <string.h>
#include "keys.h"


static Keys keys[30]={{"A",SDLK_a},{"B",SDLK_b},{"C",SDLK_c},{"D",SDLK_d},
                      {"E",SDLK_e},{"F",SDLK_f},{"G",SDLK_g},{"H",SDLK_h},
                      {"I",SDLK_i},{"J",SDLK_j},{"K",SDLK_k},{"L",SDLK_m},
                      {"M",SDLK_m},{"N",SDLK_n},{"O",SDLK_o},{"P",SDLK_p},
                      {"Q",SDLK_q},{"R",SDLK_r},{"S",SDLK_s},{"T",SDLK_t},
                      {"U",SDLK_u},{"V",SDLK_v},{"W",SDLK_w},{"X",SDLK_x},
                      {"Y",SDLK_y},{"Z",SDLK_z},
                      {"UP",SDLK_UP},{"DOWN",SDLK_DOWN},{"RIGHT",SDLK_RIGHT},{"LEFT",SDLK_LEFT}};

uint16_t charToSdlk(char * key){
    int i=0;
    for(i=0;i<30;i++){
        if(!strcmp(keys[i].keyName,key))
            return keys[i].SDLK;
    }
    return 0;
}

char * SdlkToChar(uint16_t sdlk){
    int i=0;
    for(i=0;i<30;i++){
        if(keys[i].SDLK == sdlk)
            return keys[i].keyName;
    }
    return 0;
}

int writeFileKeys(char * nameFile, uint16_t * keysSelect){
    FILE * pFile = NULL;
    pFile = fopen(nameFile,"w");
	if (pFile == NULL) return 0;
    fprintf(pFile,"P1_A      : 0 : %s\n",SdlkToChar(keysSelect[0]));  //A
    fprintf(pFile,"P1_B      : 1 : %s\n",SdlkToChar(keysSelect[1]));  //B
    fprintf(pFile,"P1_SELECT : 2 : %s\n",SdlkToChar(keysSelect[2]));  //SELECT
    fprintf(pFile,"P1_START  : 3 : %s\n",SdlkToChar(keysSelect[3]));  //START
    fprintf(pFile,"P1_UP     : 4 : %s\n",SdlkToChar(keysSelect[4]));  //UP
    fprintf(pFile,"P1_DOWN   : 5 : %s\n",SdlkToChar(keysSelect[5]));  //DOWN
    fprintf(pFile,"P1_LEFT   : 6 : %s\n",SdlkToChar(keysSelect[6]));  //LEFT
    fprintf(pFile,"P1_RIGHT  : 7 : %s\n",SdlkToChar(keysSelect[7]));  //RIGHT
    fprintf(pFile,"P2_A      : 8 : %s\n",SdlkToChar(keysSelect[8]));  //A
    fprintf(pFile,"P2_B      : 9 : %s\n",SdlkToChar(keysSelect[9]));  //B
    fprintf(pFile,"P2_SELECT : 10 : %s\n",SdlkToChar(keysSelect[10]));  //SELECT
    fprintf(pFile,"P2_START  : 11 : %s\n",SdlkToChar(keysSelect[11]));  //START
    fprintf(pFile,"P2_UP     : 12 : %s\n",SdlkToChar(keysSelect[12]));  //UP
    fprintf(pFile,"P2_DOWN   : 13 : %s\n",SdlkToChar(keysSelect[13]));  //DOWN
    fprintf(pFile,"P2_LEFT   : 14 : %s\n",SdlkToChar(keysSelect[14]));  //LEFT
    fprintf(pFile,"P2_RIGHT  : 15 : %s\n",SdlkToChar(keysSelect[15]));  //RIGHT
    fclose(pFile);
	return 1;
}


int readFileKeys(char * nameFile, uint16_t * keysSelect){
    FILE * pFile = NULL;
    char temp[30];
    char key[10];
    char buf[10];
    int i;
    pFile = fopen(nameFile,"r");
	if (pFile == NULL) return 0;
    while (fgets(temp,30,pFile)) {
        sscanf(temp,"%s : %d : %s",buf,&i,key);
        keysSelect[i]=charToSdlk(key);
    }
    fclose(pFile);
    return 1;
}

uint16_t eventKeys(uint16_t * keysSelect){
	static SDL_Event event;
    uint16_t keys;
	while(SDL_PollEvent(&event))
    {
        if(event.key.keysym.sym == keysSelect[0])
    	   keys |= 0x0001;
    	if(event.key.keysym.sym == keysSelect[1])
    	   keys |= 0x0002;
    	if(event.key.keysym.sym == keysSelect[2])
    	   keys |= 0x0004;
    	if(event.key.keysym.sym == keysSelect[3])
    	   keys |= 0x0008;
    	if(event.key.keysym.sym == keysSelect[4])
    	   keys |= 0x0010;
    	if(event.key.keysym.sym == keysSelect[5])
    	   keys |= 0x0020;
    	if(event.key.keysym.sym == keysSelect[6])
    	   keys |= 0x0040;
    	if(event.key.keysym.sym == keysSelect[7])
    	   keys |= 0x0080;
        if(event.key.keysym.sym == keysSelect[8])
       	   keys |= 0x0100;
       	if(event.key.keysym.sym == keysSelect[9])
       	   keys |= 0x0200;
       	if(event.key.keysym.sym == keysSelect[10])
       	   keys |= 0x0400;
       	if(event.key.keysym.sym == keysSelect[11])
       	   keys |= 0x0800;
       	if(event.key.keysym.sym == keysSelect[12])
       	   keys |= 0x1000;
       	if(event.key.keysym.sym == keysSelect[13])
       	   keys |= 0x2000;
       	if(event.key.keysym.sym == keysSelect[14])
       	   keys |= 0x4000;
       	if(event.key.keysym.sym == keysSelect[15])
       	   keys |= 0x8000;
    }
    return keys;
}

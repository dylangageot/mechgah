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
    fprintf(pFile,"A      : 0 : %s\n",SdlkToChar(keysSelect[0]));  //A
    fprintf(pFile,"B      : 1 : %s\n",SdlkToChar(keysSelect[1]));  //A
    fprintf(pFile,"SELECT : 2 : %s\n",SdlkToChar(keysSelect[2]));  //A
    fprintf(pFile,"START  : 3 : %s\n",SdlkToChar(keysSelect[3]));  //A
    fprintf(pFile,"UP     : 4 : %s\n",SdlkToChar(keysSelect[4]));  //A
    fprintf(pFile,"DOWN   : 5 : %s\n",SdlkToChar(keysSelect[5]));  //A
    fprintf(pFile,"LEFT   : 6 : %s\n",SdlkToChar(keysSelect[6]));  //A
    fprintf(pFile,"RIGHT  : 7 : %s\n",SdlkToChar(keysSelect[7]));  //A
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

uint8_t eventKeys(uint16_t * keysSelect){
	static SDL_Event event;
    uint8_t keys;
	while(SDL_PollEvent(&event))
    {
        if(event.key.keysym.sym == keysSelect[0])
    	   keys |= 0x01;
    	if(event.key.keysym.sym == keysSelect[1])
    	   keys |= 0x02;
    	if(event.key.keysym.sym == keysSelect[2])
    	   keys |= 0x04;
    	if(event.key.keysym.sym == keysSelect[3])
    	   keys |= 0x08;
    	if(event.key.keysym.sym == keysSelect[4])
    	   keys |= 0x10;
    	if(event.key.keysym.sym == keysSelect[5])
    	   keys |= 0x20;
    	if(event.key.keysym.sym == keysSelect[6])
    	   keys |= 0x40;
    	if(event.key.keysym.sym == keysSelect[7])
    	   keys |= 0x80;
    }
    return keys;
}

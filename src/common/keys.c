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

int charToSdlk(char * key){
    int i=0;
    for(i=0;i<30;i++){
        if(!strcmp(keys[i].keyName,key))
            return keys[i].SDLK;
    }
    return 0;
}

#ifndef KEYS_H
#define KEYS_H

typedef struct{
    char * keyName;
    int SDLK;
}Keys;

uint16_t charToSdlk(char * key);
/* Translated string in SDLK
   Input : char * key :  the name of the key
   Output : Value of the SDLK_*/

char * SdlkToChar(uint16_t sdlk);
/* Translated SDLK in string
   Input : int sdlk :  SDLK code for a key
   Output :  string */

int writeFileKeys(char * nameFile, uint16_t * keysSelect);
/* Writes the key configuration to a file
   Input : char * nameFile :  name of the file
           int * keysSelect : Table containing the SDLK keys
   Output : 1 or 0*/

int readFileKeys(char * nameFile, uint16_t * keysSelect);
/* Read the key configuration to a file
   Input : char * nameFile :  name of the file
           int * keysSelect : Table containing the SDLK keys
   Output : 1 or 0*/


int handleKeys(uint16_t * keysSelect, uint16_t * keysPressed, SDL_Event * event);
/* detect a event
    Input :  int * keysSelect : Table containing the SDLK keys
    Ouput : a value in 1 byte*/

#endif /* KEYS_H */

#ifndef KEYS_H
#define KEYS_H

typedef struct{
    char * keyName;
    int SDLK;
}Keys;

int charToSdlk(char * key);
/* Translated string in SDLK
   Input : char * key :  the name of the key
   Output : Value of the SDLK_*/

char * SdlkToChar(int sdlk);
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


int eventKeys(uint16_t * keysSelect);
/* detect a event
    Input :  int * keysSelect : Table containing the SDLK keys
    Ouput : a value in 1 byte*/

#endif /* KEYS_H */

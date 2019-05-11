/**
 * \file keys.h
 * \brief header file for Keys module
 * \author Nicolas Hily
 * \version 1.0
 * \date 2019-05-11
 */

#ifndef KEYS_H
#define KEYS_H

/**
 * \brief Hold string for key name and corresponding SDLK constant
 */
typedef struct{
    char * keyName;
    int SDLK;
}Keys;

/**
 * \brief Translate string into SDLK
 *
 * \param key name of the key
 *
 * \return value of the SDLK constant
 */
uint16_t charToSdlk(char * key);

/**
 * \brief Translate SDLK into string
 *
 * \param sdlk SDLK code for a key
 *
 * \return string pointer
 */
char * SdlkToChar(uint16_t sdlk);

/**
 * \brief Write key configuration to a file
 *
 * \param nameFile filename
 * \param keysSelect array containing SDLK keys
 *
 * \return EXIT_SUCCES if succeed, EXIT_FAILURE otherwise 
 */
int writeFileKeys(char * nameFile, uint16_t * keysSelect);

/**
 * \brief Reade key configuration from a file
 *
 * \param nameFile filename
 * \param keysSelect array containing SDLK keys 
 *
 * \return EXIT_SUCCES if succeed, EXIT_FAILURE otherwise 
 */
int readFileKeys(char * nameFile, uint16_t * keysSelect);

/**
 * \brief Handle SDL key event
 *
 * \param keysSelect array containg key configuration
 * \param keysPressed keys pressed
 * \param event SDL event
 *
 * \return 1 if app has not been closed, 0 otherwise 
 */
int handleKeys(uint16_t * keysSelect, uint16_t * keysPressed, SDL_Event * event);

#endif /* KEYS_H */

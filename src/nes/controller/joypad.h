/**
 * \file joypad.h
 * \brief header file of Joypad module
 * \author Nicolas Chabanis
 * \version 1.0
 * \date 2019-05-11
 */

 #ifndef JOYPAD_H
 #define JOYPAD_H

#include <stdint.h>

 /**
  * \brief Holds necessary variables for one joypad
 */
 typedef struct{
   int id; /* 1 for joy1, 2 for joy2 */
   int polling; /* 1=game is polling keys */
   int shiftState; /* Holds the number of stateRegister's shifts */
   uint8_t stateRegister; /* Represent the 74LS165 8-bit shift register in the joypad */
 } Joypad;

 /**
  * \brief Allocates memory for joypad
  * \param which joypad to create
  * \return instance of Joypad allocated
 */
Joypad* Joypad_Create(int id);

/**
 * \brief Latches the current pressed keys into the stateRegister
 * and writes the value of A on $401x[0] (x=6 for joy1, 7 for joy2)
 * \param self instance of the Joypad
 * \param values of the pressed keys
 * \return new value of JOYx[0]
*/
uint8_t Joypad_Latch(Joypad *self, uint16_t keysPressed);

/**
 * \brief returns the polling variable
 * \param self instance of the Joypad
 * \return value of polling variable
*/
int Joypad_getPolling(Joypad *self);

/**
 * \brief Shifts once the stateRegister into $401x[0] (x=6 for joy1, 7 for joy2)
 * \param self instance of the Joypad
 * \return new value of JOYx[0]
*/
uint8_t Joypad_Shift(Joypad *self);

 /**
  * \brief Frees the Joypad instance's memory
  * \param self instance of the Joypad
 */
 void Joypad_Destroy(Joypad *self);

 #endif /* JOYPAD_H */

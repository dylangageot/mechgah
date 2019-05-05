/**
 * \file controller.h
 * \brief Header for Controller structure
 * \author Nicolas Chabanis
 * \version 1.0
 * \date 2019-05-02
 */

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "../mapper/mapper.h"

/**
 * \struct Controller
 * \brief Holds necessary variables for the controller emulation to work.
 * Only support the standard joypads
*/
typedef struct{
  int lastState;
  int shiftState;
  int polling;
  uint16_t keys;
  Mapper *mapper;
  uint8_t JOY1;
  uint8_t JOY2;
} Controller;

/**
 * \fn Controller_Create
 * \brief Allocates memory for controller
 * \param pointer to the used mapper
 * \return instance of Controller allocated
*/
Controller* Controller_Create(Mapper *mapper);

/**
 * \fn Controller_Execute
 * \brief Executes the controller system
 * \param The instance of the controller
 * \param The state of the current pressed keys (2*8 keys)
*/
void Controller_Execute(Controller *self, uint16_t keys);

/**
 * \fn Controller_Delete
 * \brief Frees the controller instance's memory
 * \param self instance of the Controller
*/
void Controller_Destroy(Controller *self);

#endif /* CONTROLLER_H */

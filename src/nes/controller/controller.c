#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "controller.h"
#include "../../common/macro.h"
#include "../mapper/ioreg.h"
#include "joypad.h"

/* Useful documentation : http://nesdev.parodius.com/ndox200.zip
  Joypad #1 and #2 are accessed via $4016 and $4017, respectively.

  The joypads are reset via a strobing-method: writing (STA) 1, then 0, to
  $4016.

  On a full strobe, the joypad's button status will be returned in a
  single-bit stream (D0). Multiple reads (LDA) need to be made to read all the
  information about the controller.

    1 = A
    2 = B
    3 = SELECT
    4 = START
    5 = UP
    6 = DOWN
    7 = LEFT
    8 = RIGHT
*/

Controller* Controller_Create(Mapper *mapper){
  Controller * self = (Controller*)malloc(sizeof(Controller));
  if(self != NULL){
    self->mapper = mapper;
    self->joy1 = Joypad_Create(1);
    self->joy2 = Joypad_Create(2);
    /* Reset variables */
    self->lastState = -1;
    self->keysPressed = 0;
  }else{
    ERROR_MSG("can't allocate Controller structure");
  }
  return self;
}

void Controller_Execute(Controller *self, uint16_t keysPressed){
  uint8_t ackJoy1 = Mapper_Ack(self->mapper, 0x4016);
  uint8_t ackJoy2 = Mapper_Ack(self->mapper, 0x4017);
  if(ackJoy1 & AC_WR){ /* if $4016 was written to */
    if( (self->JOY1 & 0x01) == 0){
      if(self->lastState == 1){ /* if game asks to poll keys */
        self->keysPressed = keysPressed;
        self->JOY1 &= ~(0x01);
        self->JOY1 |= Joypad_Latch(self->joy1, self->keysPressed);
        self->JOY2 &= ~(0x01);
        self->JOY2 |= Joypad_Latch(self->joy2, self->keysPressed);
      }
    }else{
      self->lastState = 1;
    }
  }
  if(Joypad_getPolling(self->joy1) && (ackJoy1 & AC_RD)){ // if joy1 is being polled
    self->JOY1 &= ~(0x01);
    self->JOY1 |= Joypad_Shift(self->joy1);
  }
  if(Joypad_getPolling(self->joy2) && (ackJoy2 & AC_RD)){ // if joy2 is being polled
    self->JOY2 &= ~(0x01);
    self->JOY2 |= Joypad_Shift(self->joy2);
  }
}

void Controller_Destroy(Controller *self){
  if(self == NULL){
    return;
  }
  Joypad_Destroy(self->joy1);
  Joypad_Destroy(self->joy2);
  free(self);
}

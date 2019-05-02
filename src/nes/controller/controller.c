#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "controller.h"
#include "../../common/macro.h"
#include "../mapper/ioreg.h"

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
    /* Reset variables */
    self->lastState = -1;
    self->shiftState = 1;
    self->polling = 0;
    self->keys = 0;
  }else{
    ERROR_MSG("can't allocate Controller structure");
  }
  return self;
}

void Controller_Execute(Controller *self, uint16_t keys){
  uint8_t ack = Mapper_Ack(self->mapper, 0x4016);
  if(ack & AC_WR){
    if( (*Mapper_Get(self->mapper, AS_CPU, 0x4016) & 0x01) == 0){
      if(self->lastState == 1){
        self->polling=1;
        self->shiftState = 1;
        self->keys = keys;
        *(Mapper_Get(self->mapper, AS_CPU, 0x4016)) |= (self->keys & 0x01);
        self->shiftState++;
        self->keys = self->keys >> 1;
      }
    }else{
      self->lastState = 1;
    }
  }
  if(self->polling && (ack & AC_RD)){
    if(self->shiftState < 9){
      *(Mapper_Get(self->mapper, AS_CPU, 0x4016)) &= ~(0x01);
      *(Mapper_Get(self->mapper, AS_CPU, 0x4016)) |= (self->keys & 0x01);
      self->keys = self->keys >> 1;
      self->shiftState++;
    }else if(self->shiftState == 9){
      *(Mapper_Get(self->mapper, AS_CPU, 0x4016)) |= 0x01;
      self->shiftState++;
    }else{ //shiftState == 10
      self->shiftState = 1;
      self->keys = 0;
      self->polling = 0;
      }
  }
}

void Controller_Destroy(Controller *self){
  if(self == NULL){
    return;
  }
  free(self);
}

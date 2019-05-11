#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "joypad.h"
#include "../../common/macro.h"

Joypad* Joypad_Create(int id){
  Joypad * self = (Joypad*)malloc(sizeof(Joypad));
  if(self != NULL){
    self->id = id;
    self->polling = 0;
    self->shiftState = 1;
    self->stateRegister = 0;
  }else{
    ERROR_MSG("can't allocate Joypad structure");
  }
  return self;
}

uint8_t Joypad_Latch(Joypad *self, uint16_t keysPressed){
  self->polling = 1;
  self->shiftState = 1;
  if(self->id == 1){
    self->stateRegister = (keysPressed & 0xFF);
  }else if(self->id == 2){
    self->stateRegister = ((keysPressed & 0xFF00) >> 8);
  }
  self->shiftState++;
  return (self->stateRegister & 0x01);
}

int Joypad_getPolling(Joypad *self){
  return self->polling;
}

uint8_t Joypad_Shift(Joypad *self){
  self->stateRegister = self->stateRegister >> 1;
  if(self->shiftState < 9){
    self->shiftState++;
    return (self->stateRegister & 0x01);
  }else if(self->shiftState == 9){
    self->shiftState++;
    return 0x01;
  }else{ //shiftState == 10
    self->polling = 0;
    self->shiftState = 1;
    self->stateRegister = 0;
    }
}

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "controller.h"
#include "../../common/keys.h"
#include "../mapper/mapper.h"
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

void controllerPollInputs(NES *self, uint16_t * keysSelect){
  static int lastState = -1;
  static int shiftState = 1;
  static int polling = 0;
  static uint8_t keys = 0;
  uint8_t ack;

  ack = Mapper_Ack(self->mapper, 0x4016);

  if(ack & AC_WR){
    if( (*Mapper_Get(self->mapper, AS_CPU, 0x4016) & 0x01) == 0){
      if(lastState == 1){
        polling=1;
        shiftState = 1;
        keys = eventKeys(keysSelect);
        //if(keys != 0)
        //  printf("%d\n", keys);
        *(Mapper_Get(self->mapper, AS_CPU, 0x4016)) |= (keys & 0x01);
        shiftState++;
        keys = keys >> 1;
      }
    }else{
      lastState = 1;
    }
  }

  if(polling && (ack & AC_RD)){
    if(shiftState < 9){
      *(Mapper_Get(self->mapper, AS_CPU, 0x4016)) &= ~(0x01);
      *(Mapper_Get(self->mapper, AS_CPU, 0x4016)) |= (keys & 0x01);
      keys = keys >> 1;
      shiftState++;
    }else if(shiftState == 9){
      *(Mapper_Get(self->mapper, AS_CPU, 0x4016)) |= 0x01;
      shiftState++;
    }else{ //shiftState == 10
      shiftState = 1;
      keys = 0;
      polling = 0;
      }
  }

}

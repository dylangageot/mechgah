#include "UTest.h"
#include "../nes/controller/controller.h"
#include "../nes/mapper/nrom.h"

static int setup_Controller(void **state) {
  /* create a NROM Mapper*/
  Header config;
  config.mirroring = NROM_HORIZONTAL;
  config.romSize = NROM_16KIB;
  Mapper *mapper = MapNROM_Create(&config);

  Controller * ctrl = (Controller*)Controller_Create(mapper);
  *state = ctrl;
  if(*state == NULL){
    return -1;
  }

  IOReg * ioreg = IOReg_Extract(mapper);

  ioreg->bank2[JOY1] = &(ctrl->JOY1);
  ioreg->bank2[JOY2] = &(ctrl->JOY2);

  return 0;
}

static void test_Controller_Execute(void ** state){
  Controller * ctrl = (Controller*)*state;
  uint16_t keysPressed = 0x1B1B;
  //uint16_t keysPressed = 0x0000;
  uint8_t keysPressedVerify1 = keysPressed & 0xFF;
  uint8_t keysPressedVerify2 = ((keysPressed & 0xFF00) >> 8);

  /* Keys were not asked for */
  Controller_Execute(ctrl, keysPressed);
  assert_int_equal(ctrl->JOY1,0x01);
  assert_int_equal(ctrl->JOY2,0x01);

  /* Game asks for keys */
  *Mapper_Get(ctrl->mapper, AS_CPU | AC_WR, 0x4016) = 0x01;
  Controller_Execute(ctrl, keysPressed);
  *Mapper_Get(ctrl->mapper, AS_CPU | AC_WR, 0x4016) = 0x00;
  Controller_Execute(ctrl, keysPressed);
  assert_int_equal(ctrl->JOY1,keysPressedVerify1 & 0x01);
  assert_int_equal(ctrl->JOY2,keysPressedVerify1 & 0x01);

  /* joy1 ($4016) */
  keysPressedVerify1 = keysPressedVerify1 >> 1; /* joy1 was latched */
  for(int i=0 ; i<5 ; i++){
    Mapper_Get(ctrl->mapper, AS_CPU | AC_RD, 0x4016);
    Controller_Execute(ctrl, keysPressed);
    assert_int_equal(ctrl->JOY1,keysPressedVerify1 & 0x01);
    keysPressedVerify1 = keysPressedVerify1 >> 1;
  }

  /* $4016 was not read, JOY1 still holds the key. Tested for two reads */
  /* Simulates CPU execution while game isn't asking for keys*/
  Controller_Execute(ctrl, keysPressed);
  assert_int_equal(ctrl->JOY1,keysPressedVerify1 & 0x01);
  Controller_Execute(ctrl, keysPressed);
  assert_int_equal(ctrl->JOY1,keysPressedVerify1 & 0x01);

  /* Try reading joy2 ($4017) in the middle */
  keysPressedVerify2 = keysPressedVerify2 >> 1; /* joy2 was latched */
  Mapper_Get(ctrl->mapper, AS_CPU | AC_RD, 0x4017);
  Controller_Execute(ctrl, keysPressed);
  assert_int_equal(ctrl->JOY2,keysPressedVerify2 & 0x01);
  keysPressedVerify2 = keysPressedVerify2 >> 1;

  /* joy1 ($4016) */
  for(int i=5 ; i<7 ; i++){
    Mapper_Get(ctrl->mapper, AS_CPU | AC_RD, 0x4016);
    Controller_Execute(ctrl, keysPressed);
    assert_int_equal(ctrl->JOY1,keysPressedVerify1 & 0x01);
    keysPressedVerify1 = keysPressedVerify1 >> 1;
  }

  /* JOY2 was not read, JOY1 is finished polling, reading gives 1 */
  Mapper_Get(ctrl->mapper, AS_CPU | AC_RD, 0x4016);
  Controller_Execute(ctrl, keysPressed);
  assert_int_equal(ctrl->JOY1,0x01);

  /* joy2 ($4017) */
  for(int i=0 ; i<6 ; i++){
    Mapper_Get(ctrl->mapper, AS_CPU | AC_RD, 0x4017);
    Controller_Execute(ctrl, keysPressed);
    assert_int_equal(ctrl->JOY2,keysPressedVerify2 & 0x01);
    keysPressedVerify2 = keysPressedVerify2 >> 1;
  }

  /* JOY2 is finished polling, reading gives 1 */
  Mapper_Get(ctrl->mapper, AS_CPU | AC_RD, 0x4017);
  Controller_Execute(ctrl, keysPressed);
  assert_int_equal(ctrl->JOY2,0x01);
}

static int teardown_Controller(void **state) {
  Controller * ctrl = (Controller*)*state;
  if(*state != NULL){
    Mapper_Destroy(ctrl->mapper);
    Controller_Destroy(ctrl);

    return 0;
  }else{
    return -1;
  }
}

int run_UTcontroller(void) {
  const struct CMUnitTest test_Controller[] = {
    cmocka_unit_test(test_Controller_Execute)
  };
  int out = 0;
  out += cmocka_run_group_tests(test_Controller, setup_Controller, teardown_Controller);
  return out;
}

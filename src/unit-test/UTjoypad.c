#include "UTest.h"
#include "../nes/controller/joypad.h"

static int setup_Joypad1(void **state) {
  *state = (Joypad*)Joypad_Create(1);
  if(*state == NULL){
    return -1;
  }
  return 0;
}

static int setup_Joypad2(void **state) {
  *state = (Joypad*)Joypad_Create(2);
  if(*state == NULL){
    return -1;
  }
  return 0;
}

static void test_JoypadLatch1(void ** state){
  Joypad * joy = (Joypad*)*state;
  uint8_t JOY = 0;
  uint16_t keysPressed = 0x0101;
  assert_int_equal(joy->polling,0);
  assert_int_equal(joy->shiftState,1);
  JOY |= Joypad_Latch(joy, keysPressed);
  assert_int_equal(joy->stateRegister, 0x01);
  assert_int_equal(joy->polling,1);
  assert_int_equal(joy->shiftState,2);
  assert_int_equal(JOY,1);
}

static void test_JoypadLatch0(void ** state){
  Joypad * joy = (Joypad*)*state;
  uint8_t JOY = 0;
  uint16_t keysPressed = 0xFEFE;
  assert_int_equal(joy->polling,0);
  assert_int_equal(joy->shiftState,1);
  JOY |= Joypad_Latch(joy, keysPressed);
  assert_int_equal(joy->stateRegister, 0xFE);
  assert_int_equal(joy->polling,1);
  assert_int_equal(joy->shiftState,2);
  assert_int_equal(JOY,0);
}

static void test_JoypadShift(void ** state){
  Joypad * joy = (Joypad*)*state;
  uint8_t JOY = 0;
  uint16_t keysPressed = 0x1B1B; /* stateRegister is 0001 1011 (b) */
  JOY |= Joypad_Latch(joy,keysPressed);

  JOY &= ~(0x01);
  JOY |= Joypad_Shift(joy);
  assert_int_equal(JOY, 1);
  assert_int_equal(joy->shiftState, 3);
  assert_int_equal(joy->stateRegister, 0x0D); /* 0001 1011 -> 0000 1101 */

  JOY &= ~(0x01);
  JOY |= Joypad_Shift(joy);
  assert_int_equal(JOY, 0);
  assert_int_equal(joy->shiftState, 4);
  assert_int_equal(joy->stateRegister, 0x06); /* 0000 1101 -> 0000 0110 */

  JOY &= ~(0x01);
  JOY |= Joypad_Shift(joy);
  assert_int_equal(JOY, 1);
  assert_int_equal(joy->shiftState, 5);
  assert_int_equal(joy->stateRegister, 0x03); /* 0000 0110 -> 0000 0011 */

  JOY &= ~(0x01);
  JOY |= Joypad_Shift(joy);
  assert_int_equal(JOY, 1);
  assert_int_equal(joy->shiftState, 6);
  assert_int_equal(joy->stateRegister, 0x01); /* 0000 0011 -> 0000 0001 */

  JOY &= ~(0x01);
  JOY |= Joypad_Shift(joy);
  assert_int_equal(JOY, 0);
  assert_int_equal(joy->shiftState, 7);
  assert_int_equal(joy->stateRegister, 0x00); /* 0000 0001 -> 0000 0000 */

  JOY &= ~(0x01);
  JOY |= Joypad_Shift(joy);
  assert_int_equal(JOY, 0);
  assert_int_equal(joy->shiftState, 8);
  assert_int_equal(joy->stateRegister, 0x00); /* 0000 0000 -> 0000 0000 */

  JOY &= ~(0x01);
  JOY |= Joypad_Shift(joy);
  assert_int_equal(JOY, 0);
  assert_int_equal(joy->shiftState, 9);
  assert_int_equal(joy->stateRegister, 0x00); /* 0000 0000 -> 0000 0000 */

  JOY &= ~(0x01);
  JOY |= Joypad_Shift(joy);
  assert_int_equal(JOY, 1); /* shiftState was 9 */
  assert_int_equal(joy->shiftState, 10);
  assert_int_equal(joy->stateRegister, 0x00); /* 0000 0000 -> 0000 0000 */

  JOY &= ~(0x01);
  JOY |= Joypad_Shift(joy);
  assert_int_equal(JOY, 1); /* shiftState was 10 */
  assert_int_equal(joy->polling, 0);
  assert_int_equal(joy->shiftState, 1);
  assert_int_equal(joy->stateRegister, 0x00);
}

static void test_JoypadGetPolling(void ** state){
  Joypad * joy = (Joypad*)*state;
  assert_int_equal(Joypad_getPolling(joy), 0);
  Joypad_Latch(joy, 0x0000);
  assert_int_equal(Joypad_getPolling(joy), 1);
}

static int teardown_Joypad(void **state) {
  Joypad * joy = (Joypad*)*state;
  if(*state != NULL){
    Joypad_Destroy(joy);
    return 0;
  }else{
    return -1;
  }
}

int run_UTjoypad(void) {
  const struct CMUnitTest test_Latch0[] = {
    cmocka_unit_test(test_JoypadLatch0)
  };
  const struct CMUnitTest test_Latch1[] = {
    cmocka_unit_test(test_JoypadLatch1)
  };
  const struct CMUnitTest test_Joypad[] = {
    cmocka_unit_test(test_JoypadShift),
    cmocka_unit_test(test_JoypadGetPolling)
  };
  int out = 0;
  out += cmocka_run_group_tests(test_Latch0, setup_Joypad1, teardown_Joypad);
  out += cmocka_run_group_tests(test_Latch0, setup_Joypad2, teardown_Joypad);
  out += cmocka_run_group_tests(test_Latch1, setup_Joypad1, teardown_Joypad);
  out += cmocka_run_group_tests(test_Latch1, setup_Joypad2, teardown_Joypad);
  out += cmocka_run_group_tests(test_Joypad, setup_Joypad1, teardown_Joypad);
  out += cmocka_run_group_tests(test_Joypad, setup_Joypad2, teardown_Joypad);
  return out;
}

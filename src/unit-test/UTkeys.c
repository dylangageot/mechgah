#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include "UTest.h"
#include "../common/keys.h"

static void test_charToSdlk() {
    /*Test de bon fonctionnement de fonction et de la bonne association
    dans le tableau des touches static*/
    assert_int_equal(SDLK_a,charToSdlk("A"));
    assert_int_equal(SDLK_b,charToSdlk("B"));
    assert_int_equal(SDLK_c,charToSdlk("C"));
    assert_int_equal(SDLK_d,charToSdlk("D"));
    assert_int_equal(SDLK_e,charToSdlk("E"));
    assert_int_equal(SDLK_f,charToSdlk("F"));
    assert_int_equal(SDLK_g,charToSdlk("G"));
    assert_int_equal(SDLK_h,charToSdlk("H"));
    assert_int_equal(SDLK_i,charToSdlk("I"));
    assert_int_equal(SDLK_j,charToSdlk("J"));
    assert_int_equal(SDLK_k,charToSdlk("K"));
    assert_int_equal(SDLK_l,charToSdlk("L"));
    assert_int_equal(SDLK_m,charToSdlk("M"));
    assert_int_equal(SDLK_n,charToSdlk("N"));
    assert_int_equal(SDLK_o,charToSdlk("O"));
    assert_int_equal(SDLK_p,charToSdlk("P"));
    assert_int_equal(SDLK_q,charToSdlk("Q"));
    assert_int_equal(SDLK_r,charToSdlk("R"));
    assert_int_equal(SDLK_s,charToSdlk("S"));
    assert_int_equal(SDLK_t,charToSdlk("T"));
    assert_int_equal(SDLK_u,charToSdlk("U"));
    assert_int_equal(SDLK_v,charToSdlk("V"));
    assert_int_equal(SDLK_w,charToSdlk("W"));
    assert_int_equal(SDLK_x,charToSdlk("X"));
    assert_int_equal(SDLK_y,charToSdlk("Y"));
    assert_int_equal(SDLK_z,charToSdlk("Z"));
    assert_int_equal(SDLK_UP,charToSdlk("UP"));
    assert_int_equal(SDLK_DOWN,charToSdlk("DOWN"));
    assert_int_equal(SDLK_RIGHT,charToSdlk("RIGHT"));
    assert_int_equal(SDLK_LEFT,charToSdlk("LEFT"));
    /*Test pour une valeur inexistante*/
    assert_int_equal(0,charToSdlk("FOO"));
}


int run_UTkeys(void){
  const struct CMUnitTest test_file[] = {
    cmocka_unit_test(test_charToSdlk),
  };
  int out = 0;
  out += cmocka_run_group_tests(test_file, NULL, NULL);
  return out;
}

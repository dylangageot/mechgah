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

static void test_SdlkToChar() {
    /*Test de bon fonctionnement de fonction et de la bonne association
    dans le tableau des touches static*/
    assert_string_equal("A",SdlkToChar(SDLK_a));
    assert_string_equal("B",SdlkToChar(SDLK_b));
    assert_string_equal("C",SdlkToChar(SDLK_c));
    assert_string_equal("D",SdlkToChar(SDLK_d));
    assert_string_equal("E",SdlkToChar(SDLK_e));
    assert_string_equal("F",SdlkToChar(SDLK_f));
    assert_string_equal("G",SdlkToChar(SDLK_g));
    assert_string_equal("H",SdlkToChar(SDLK_h));
    assert_string_equal("I",SdlkToChar(SDLK_i));
    assert_string_equal("J",SdlkToChar(SDLK_j));
    assert_string_equal("K",SdlkToChar(SDLK_k));
    assert_string_equal("L",SdlkToChar(SDLK_l));
    assert_string_equal("M",SdlkToChar(SDLK_m));
    assert_string_equal("N",SdlkToChar(SDLK_n));
    assert_string_equal("O",SdlkToChar(SDLK_o));
    assert_string_equal("P",SdlkToChar(SDLK_p));
    assert_string_equal("Q",SdlkToChar(SDLK_q));
    assert_string_equal("R",SdlkToChar(SDLK_r));
    assert_string_equal("S",SdlkToChar(SDLK_s));
    assert_string_equal("T",SdlkToChar(SDLK_t));
    assert_string_equal("U",SdlkToChar(SDLK_u));
    assert_string_equal("V",SdlkToChar(SDLK_v));
    assert_string_equal("W",SdlkToChar(SDLK_w));
    assert_string_equal("X",SdlkToChar(SDLK_x));
    assert_string_equal("Y",SdlkToChar(SDLK_y));
    assert_string_equal("Z",SdlkToChar(SDLK_z));
    assert_string_equal("UP",SdlkToChar(SDLK_UP));
    assert_string_equal("DOWN",SdlkToChar(SDLK_DOWN));
    assert_string_equal("RIGHT",SdlkToChar(SDLK_RIGHT));
    assert_string_equal("LEFT",SdlkToChar(SDLK_LEFT));
    /*Test pour une valeur inexistante dans le tableau*/
    assert_ptr_equal(NULL,SdlkToChar(SDLK_ESCAPE));
}

static void test_readFileKeys(){
    uint16_t keysSelect[16];
    /*Test du fonctionnement de la fonction avec un bonne affection des touches*/
    assert_int_equal(1,readFileKeys("src/unit-test/TestFile/UTKeysConfig.txt",keysSelect));
    assert_int_equal(SDLK_z,keysSelect[0]);
    assert_int_equal(SDLK_a,keysSelect[1]);
    assert_int_equal(SDLK_s,keysSelect[2]);
    assert_int_equal(SDLK_q,keysSelect[3]);
    assert_int_equal(SDLK_UP,keysSelect[4]);
    assert_int_equal(SDLK_DOWN,keysSelect[5]);
    assert_int_equal(SDLK_LEFT,keysSelect[6]);
    assert_int_equal(SDLK_RIGHT,keysSelect[7]);
    assert_int_equal(SDLK_u,keysSelect[8]);
    assert_int_equal(SDLK_y,keysSelect[9]);
    assert_int_equal(SDLK_j,keysSelect[10]);
    assert_int_equal(SDLK_h,keysSelect[11]);
    assert_int_equal(SDLK_o,keysSelect[12]);
    assert_int_equal(SDLK_l,keysSelect[13]);
    assert_int_equal(SDLK_k,keysSelect[14]);
    assert_int_equal(SDLK_m,keysSelect[15]);
    /* Test pour un fichier inexistant */
    assert_int_equal(0,readFileKeys("src/unit-test/TestFile/Config.txt",keysSelect));
    /* Test pour un tableau non initialisé */
    assert_int_equal(0,readFileKeys("src/unit-test/TestFile/UTKeysConfig.txt",NULL));
    /* Test en cas d'erreur d'écriture dans le fichier */
    assert_int_equal(0,readFileKeys("src/unit-test/TestFile/UTKeysConfigERR1.txt",NULL));
}

static void test_writeFileKeys(){
    uint16_t keysSelect[16];
    readFileKeys("src/unit-test/TestFile/UTKeysConfig.txt",keysSelect);
    assert_int_equal(1,writeFileKeys("src/unit-test/TestFile/UTKeysConfig.txt",keysSelect));
    assert_int_equal(0,writeFileKeys("/unit-test/TestFile/UTKeysConfig.txt",keysSelect));
    assert_int_equal(0,writeFileKeys("/unit-test/TestFile/UTKeys.txt",NULL));
}

int __wrap_SDL_PollEvent(SDL_Event * event){
    /*Focnction de wrap de SDL_PollEvent */
    event->type = mock_type(int);
    if((event->type == SDL_KEYDOWN)||(event->type == SDL_KEYUP))
        event->key.keysym.sym = mock_type(int);
    return mock_type(int);
}

static void test_handleKeys(){
    uint16_t keysSelect[16];
    uint16_t keysPressed=0x0000;
    SDL_Event event;
    readFileKeys("src/unit-test/TestFile/UTKeysConfig.txt",keysSelect);
    /* Test du bon fonctionnement de la fonction handleKeys */
    /* Création de la séqence de wrap de SDL_PollEvent pour un appui*/
    will_return(__wrap_SDL_PollEvent, SDL_KEYDOWN);
    will_return(__wrap_SDL_PollEvent, keysSelect[0]);
    will_return(__wrap_SDL_PollEvent, 1);
    /*Sequence pour sortir du while*/
    will_return(__wrap_SDL_PollEvent, 0);
    will_return(__wrap_SDL_PollEvent, 0);
    assert_int_equal(1,handleKeys(keysSelect,&keysPressed,&event));
    assert_int_equal(0x0001,keysPressed);
    /*Répétion pour les touches suivantes */
    will_return(__wrap_SDL_PollEvent, SDL_KEYDOWN);
    will_return(__wrap_SDL_PollEvent, keysSelect[1]);
    will_return(__wrap_SDL_PollEvent, 1);
    will_return(__wrap_SDL_PollEvent, 0);
    will_return(__wrap_SDL_PollEvent, 0);
    assert_int_equal(1,handleKeys(keysSelect,&keysPressed,&event));
    assert_int_equal(0x0003,keysPressed);

    will_return(__wrap_SDL_PollEvent, SDL_KEYDOWN);
    will_return(__wrap_SDL_PollEvent, keysSelect[2]);
    will_return(__wrap_SDL_PollEvent, 1);
    will_return(__wrap_SDL_PollEvent, 0);
    will_return(__wrap_SDL_PollEvent, 0);
    assert_int_equal(1,handleKeys(keysSelect,&keysPressed,&event));
    assert_int_equal(0x0007,keysPressed);

    will_return(__wrap_SDL_PollEvent, SDL_KEYDOWN);
    will_return(__wrap_SDL_PollEvent, keysSelect[3]);
    will_return(__wrap_SDL_PollEvent, 1);
    will_return(__wrap_SDL_PollEvent, 0);
    will_return(__wrap_SDL_PollEvent, 0);
    assert_int_equal(1,handleKeys(keysSelect,&keysPressed,&event));
    assert_int_equal(0x000F,keysPressed);

    will_return(__wrap_SDL_PollEvent, SDL_KEYDOWN);
    will_return(__wrap_SDL_PollEvent, keysSelect[4]);
    will_return(__wrap_SDL_PollEvent, 1);
    will_return(__wrap_SDL_PollEvent, 0);
    will_return(__wrap_SDL_PollEvent, 0);
    assert_int_equal(1,handleKeys(keysSelect,&keysPressed,&event));
    assert_int_equal(0x001F,keysPressed);

    will_return(__wrap_SDL_PollEvent, SDL_KEYDOWN);
    will_return(__wrap_SDL_PollEvent, keysSelect[5]);
    will_return(__wrap_SDL_PollEvent, 1);
    will_return(__wrap_SDL_PollEvent, 0);
    will_return(__wrap_SDL_PollEvent, 0);
    assert_int_equal(1,handleKeys(keysSelect,&keysPressed,&event));
    assert_int_equal(0x003F,keysPressed);

    will_return(__wrap_SDL_PollEvent, SDL_KEYDOWN);
    will_return(__wrap_SDL_PollEvent, keysSelect[6]);
    will_return(__wrap_SDL_PollEvent, 1);
    will_return(__wrap_SDL_PollEvent, 0);
    will_return(__wrap_SDL_PollEvent, 0);
    assert_int_equal(1,handleKeys(keysSelect,&keysPressed,&event));
    assert_int_equal(0x007F,keysPressed);

    will_return(__wrap_SDL_PollEvent, SDL_KEYDOWN);
    will_return(__wrap_SDL_PollEvent, keysSelect[7]);
    will_return(__wrap_SDL_PollEvent, 1);
    will_return(__wrap_SDL_PollEvent, 0);
    will_return(__wrap_SDL_PollEvent, 0);
    assert_int_equal(1,handleKeys(keysSelect,&keysPressed,&event));
    assert_int_equal(0x00FF,keysPressed);

    will_return(__wrap_SDL_PollEvent, SDL_KEYDOWN);
    will_return(__wrap_SDL_PollEvent, keysSelect[8]);
    will_return(__wrap_SDL_PollEvent, 1);
    will_return(__wrap_SDL_PollEvent, 0);
    will_return(__wrap_SDL_PollEvent, 0);
    assert_int_equal(1,handleKeys(keysSelect,&keysPressed,&event));
    assert_int_equal(0x01FF,keysPressed);

    will_return(__wrap_SDL_PollEvent, SDL_KEYDOWN);
    will_return(__wrap_SDL_PollEvent, keysSelect[9]);
    will_return(__wrap_SDL_PollEvent, 1);
    will_return(__wrap_SDL_PollEvent, 0);
    will_return(__wrap_SDL_PollEvent, 0);
    assert_int_equal(1,handleKeys(keysSelect,&keysPressed,&event));
    assert_int_equal(0x03FF,keysPressed);

    will_return(__wrap_SDL_PollEvent, SDL_KEYDOWN);
    will_return(__wrap_SDL_PollEvent, keysSelect[10]);
    will_return(__wrap_SDL_PollEvent, 1);
    will_return(__wrap_SDL_PollEvent, 0);
    will_return(__wrap_SDL_PollEvent, 0);
    assert_int_equal(1,handleKeys(keysSelect,&keysPressed,&event));
    assert_int_equal(0x07FF,keysPressed);

    will_return(__wrap_SDL_PollEvent, SDL_KEYDOWN);
    will_return(__wrap_SDL_PollEvent, keysSelect[11]);
    will_return(__wrap_SDL_PollEvent, 1);
    will_return(__wrap_SDL_PollEvent, 0);
    will_return(__wrap_SDL_PollEvent, 0);
    assert_int_equal(1,handleKeys(keysSelect,&keysPressed,&event));
    assert_int_equal(0x0FFF,keysPressed);

    will_return(__wrap_SDL_PollEvent, SDL_KEYDOWN);
    will_return(__wrap_SDL_PollEvent, keysSelect[12]);
    will_return(__wrap_SDL_PollEvent, 1);
    will_return(__wrap_SDL_PollEvent, 0);
    will_return(__wrap_SDL_PollEvent, 0);
    assert_int_equal(1,handleKeys(keysSelect,&keysPressed,&event));
    assert_int_equal(0x1FFF,keysPressed);

    will_return(__wrap_SDL_PollEvent, SDL_KEYDOWN);
    will_return(__wrap_SDL_PollEvent, keysSelect[13]);
    will_return(__wrap_SDL_PollEvent, 1);
    will_return(__wrap_SDL_PollEvent, 0);
    will_return(__wrap_SDL_PollEvent, 0);
    assert_int_equal(1,handleKeys(keysSelect,&keysPressed,&event));
    assert_int_equal(0x3FFF,keysPressed);

    will_return(__wrap_SDL_PollEvent, SDL_KEYDOWN);
    will_return(__wrap_SDL_PollEvent, keysSelect[14]);
    will_return(__wrap_SDL_PollEvent, 1);
    will_return(__wrap_SDL_PollEvent, 0);
    will_return(__wrap_SDL_PollEvent, 0);
    assert_int_equal(1,handleKeys(keysSelect,&keysPressed,&event));
    assert_int_equal(0x7FFF,keysPressed);

    will_return(__wrap_SDL_PollEvent, SDL_KEYDOWN);
    will_return(__wrap_SDL_PollEvent, keysSelect[15]);
    will_return(__wrap_SDL_PollEvent, 1);
    will_return(__wrap_SDL_PollEvent, 0);
    will_return(__wrap_SDL_PollEvent, 0);
    assert_int_equal(1,handleKeys(keysSelect,&keysPressed,&event));
    assert_int_equal(0xFFFF,keysPressed);
    /* Création de la séqence de wrap de SDL_PollEvent pour un relachement*/
    will_return(__wrap_SDL_PollEvent, SDL_KEYUP);
    will_return(__wrap_SDL_PollEvent, keysSelect[0]);
    will_return(__wrap_SDL_PollEvent, 1);
    will_return(__wrap_SDL_PollEvent, 0);
    will_return(__wrap_SDL_PollEvent, 0);
    assert_int_equal(1,handleKeys(keysSelect,&keysPressed,&event));
    assert_int_equal(0xFFFE,keysPressed);
    /*Répétion pour les touches suivantes */

    will_return(__wrap_SDL_PollEvent, SDL_KEYUP);
    will_return(__wrap_SDL_PollEvent, keysSelect[1]);
    will_return(__wrap_SDL_PollEvent, 1);
    will_return(__wrap_SDL_PollEvent, 0);
    will_return(__wrap_SDL_PollEvent, 0);
    assert_int_equal(1,handleKeys(keysSelect,&keysPressed,&event));
    assert_int_equal(0xFFFC,keysPressed);

    will_return(__wrap_SDL_PollEvent, SDL_KEYUP);
    will_return(__wrap_SDL_PollEvent, keysSelect[2]);
    will_return(__wrap_SDL_PollEvent, 1);
    will_return(__wrap_SDL_PollEvent, 0);
    will_return(__wrap_SDL_PollEvent, 0);
    assert_int_equal(1,handleKeys(keysSelect,&keysPressed,&event));
    assert_int_equal(0xFFF8,keysPressed);

    will_return(__wrap_SDL_PollEvent, SDL_KEYUP);
    will_return(__wrap_SDL_PollEvent, keysSelect[3]);
    will_return(__wrap_SDL_PollEvent, 1);
    will_return(__wrap_SDL_PollEvent, 0);
    will_return(__wrap_SDL_PollEvent, 0);
    assert_int_equal(1,handleKeys(keysSelect,&keysPressed,&event));
    assert_int_equal(0xFFF0,keysPressed);

    will_return(__wrap_SDL_PollEvent, SDL_KEYUP);
    will_return(__wrap_SDL_PollEvent, keysSelect[4]);
    will_return(__wrap_SDL_PollEvent, 1);
    will_return(__wrap_SDL_PollEvent, 0);
    will_return(__wrap_SDL_PollEvent, 0);
    assert_int_equal(1,handleKeys(keysSelect,&keysPressed,&event));
    assert_int_equal(0xFFE0,keysPressed);

    will_return(__wrap_SDL_PollEvent, SDL_KEYUP);
    will_return(__wrap_SDL_PollEvent, keysSelect[5]);
    will_return(__wrap_SDL_PollEvent, 1);
    will_return(__wrap_SDL_PollEvent, 0);
    will_return(__wrap_SDL_PollEvent, 0);
    assert_int_equal(1,handleKeys(keysSelect,&keysPressed,&event));
    assert_int_equal(0xFFC0,keysPressed);

    will_return(__wrap_SDL_PollEvent, SDL_KEYUP);
    will_return(__wrap_SDL_PollEvent, keysSelect[6]);
    will_return(__wrap_SDL_PollEvent, 1);
    will_return(__wrap_SDL_PollEvent, 0);
    will_return(__wrap_SDL_PollEvent, 0);
    assert_int_equal(1,handleKeys(keysSelect,&keysPressed,&event));
    assert_int_equal(0xFF80,keysPressed);

    will_return(__wrap_SDL_PollEvent, SDL_KEYUP);
    will_return(__wrap_SDL_PollEvent, keysSelect[7]);
    will_return(__wrap_SDL_PollEvent, 1);
    will_return(__wrap_SDL_PollEvent, 0);
    will_return(__wrap_SDL_PollEvent, 0);
    assert_int_equal(1,handleKeys(keysSelect,&keysPressed,&event));
    assert_int_equal(0xFF00,keysPressed);

    will_return(__wrap_SDL_PollEvent, SDL_KEYUP);
    will_return(__wrap_SDL_PollEvent, keysSelect[8]);
    will_return(__wrap_SDL_PollEvent, 1);
    will_return(__wrap_SDL_PollEvent, 0);
    will_return(__wrap_SDL_PollEvent, 0);
    assert_int_equal(1,handleKeys(keysSelect,&keysPressed,&event));
    assert_int_equal(0xFE00,keysPressed);

    will_return(__wrap_SDL_PollEvent, SDL_KEYUP);
    will_return(__wrap_SDL_PollEvent, keysSelect[9]);
    will_return(__wrap_SDL_PollEvent, 1);
    will_return(__wrap_SDL_PollEvent, 0);
    will_return(__wrap_SDL_PollEvent, 0);
    assert_int_equal(1,handleKeys(keysSelect,&keysPressed,&event));
    assert_int_equal(0xFC00,keysPressed);

    will_return(__wrap_SDL_PollEvent, SDL_KEYUP);
    will_return(__wrap_SDL_PollEvent, keysSelect[10]);
    will_return(__wrap_SDL_PollEvent, 1);
    will_return(__wrap_SDL_PollEvent, 0);
    will_return(__wrap_SDL_PollEvent, 0);
    assert_int_equal(1,handleKeys(keysSelect,&keysPressed,&event));
    assert_int_equal(0xF800,keysPressed);

    will_return(__wrap_SDL_PollEvent, SDL_KEYUP);
    will_return(__wrap_SDL_PollEvent, keysSelect[11]);
    will_return(__wrap_SDL_PollEvent, 1);
    will_return(__wrap_SDL_PollEvent, 0);
    will_return(__wrap_SDL_PollEvent, 0);
    assert_int_equal(1,handleKeys(keysSelect,&keysPressed,&event));
    assert_int_equal(0xF000,keysPressed);

    will_return(__wrap_SDL_PollEvent, SDL_KEYUP);
    will_return(__wrap_SDL_PollEvent, keysSelect[12]);
    will_return(__wrap_SDL_PollEvent, 1);
    will_return(__wrap_SDL_PollEvent, 0);
    will_return(__wrap_SDL_PollEvent, 0);
    assert_int_equal(1,handleKeys(keysSelect,&keysPressed,&event));
    assert_int_equal(0xE000,keysPressed);

    will_return(__wrap_SDL_PollEvent, SDL_KEYUP);
    will_return(__wrap_SDL_PollEvent, keysSelect[13]);
    will_return(__wrap_SDL_PollEvent, 1);
    will_return(__wrap_SDL_PollEvent, 0);
    will_return(__wrap_SDL_PollEvent, 0);
    assert_int_equal(1,handleKeys(keysSelect,&keysPressed,&event));
    assert_int_equal(0xC000,keysPressed);

    will_return(__wrap_SDL_PollEvent, SDL_KEYUP);
    will_return(__wrap_SDL_PollEvent, keysSelect[14]);
    will_return(__wrap_SDL_PollEvent, 1);
    will_return(__wrap_SDL_PollEvent, 0);
    will_return(__wrap_SDL_PollEvent, 0);
    assert_int_equal(1,handleKeys(keysSelect,&keysPressed,&event));
    assert_int_equal(0x8000,keysPressed);

    will_return(__wrap_SDL_PollEvent, SDL_KEYUP);
    will_return(__wrap_SDL_PollEvent, keysSelect[15]);
    will_return(__wrap_SDL_PollEvent, 1);
    will_return(__wrap_SDL_PollEvent, 0);
    will_return(__wrap_SDL_PollEvent, 0);
    assert_int_equal(1,handleKeys(keysSelect,&keysPressed,&event));
    assert_int_equal(0x0000,keysPressed);
    /*Test pour quitter la sdl*/
    will_return(__wrap_SDL_PollEvent, SDL_QUIT);
    will_return(__wrap_SDL_PollEvent, 1);
    assert_int_equal(0,handleKeys(keysSelect,&keysPressed,&event));
    assert_int_equal(0x0000,keysPressed);
}

int run_UTkeys(void){
  const struct CMUnitTest test_file[] = {
      cmocka_unit_test(test_charToSdlk),
      cmocka_unit_test(test_SdlkToChar),
      cmocka_unit_test(test_readFileKeys),
      cmocka_unit_test(test_writeFileKeys),
      cmocka_unit_test(test_handleKeys)
  };
  int out = 0;
  out += cmocka_run_group_tests(test_file, NULL, NULL);
  return out;
}

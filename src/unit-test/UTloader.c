#include "UTest.h"
#include "../nes/loader/loader.h"

/*
à tester :
- filename non inexistant
- header erroné (fichier trop petit ?)
- format erroné (pas de NES^Z au début)
- rip -> tester les bit 10 à 15
- tester fillHeader
- mapper non codé
*/

static int setup_loadROM_format(void **state){

}

static int teardown_loader(void **state){
  if(*state != NULL){
    return 0;
  } else {
    return -1;
  }
}

int run_UTloader(void){
  const struct CMUnitTest test_LOADER[] = {

  }
  int out = 0;
  return out;
}

#include "UTest.h"
#include "../nes/loader/loader.h"
#include "../nes/mapper/nrom.h"
#include "NROMData.h"

static void test_loadROM_path(){
  Mapper * mapper = loadROM("nopath.nes");
  assert_int_equal(NULL,mapper);
}

static void test_loadROM_size(){
  Mapper * mapper = loadROM("src/unit-test/roms/size.nes");
  assert_int_equal(NULL,mapper);
}

static void test_loadROM_format(){
  Mapper * mapper = loadROM("src/unit-test/roms/format.nes");
  assert_int_equal(NULL,mapper);
}

static void test_loadROM_rip(){
  Mapper * mapper = loadROM("src/unit-test/roms/rip.nes");
  assert_int_equal(NULL,mapper);
}

static void test_loadROM_mapperNotDescribed(){
  Mapper * mapper = loadROM("src/unit-test/roms/nomapper.nes");
  assert_int_equal(NULL,mapper);
}

static int setup_fillHeader(void **state){
  *state = (void *) malloc(sizeof(Header));
  if (*state == NULL)
		return -1;
	return 0;
}

static void test_fillHeader(void **state){
  Header * header = (Header*)*state;
  uint8_t h[16] = { 0x4E, 0x45, 0x53, 0x1a, 0xFF, 0xFF, 0xFF, 0xFF,
                0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  fillHeader(header,h);
  assert_int_equal(header->romSize,h[4]);
  assert_int_equal(header->vromSize,h[5]);
  assert_int_equal(header->ramSize,h[8]);
  assert_int_equal(header->mapper,(h[7] & 0xF0) | ((h[6] & 0xF0)>>4));
  assert_int_equal(header->mirroring,(h[6] & 0x01));
  assert_int_equal(header->battery_backed_RAM, (h[6] & 0x02)>0);
  assert_int_equal(header->trainer,(h[6] & 0x04)>0);
  assert_int_equal(header->four_screen_VRAM, (h[6] & 0x08)>0);
  assert_int_equal(header->VS_System, (h[7] & 0x01)>0);
  assert_int_equal(header->playchoice, (h[7] & 0x02)>0);
  assert_int_equal(header->tvSystem, (h[9] & 0x01));
}

static int teardown_fillHeader(void **state){
  free((Header*)*state);
  if(*state != NULL){
    return 0;
  } else {
    return -1;
  }
}

static int setup_loadROM_NROM(void **state){
  *state = (Mapper*)loadROM("src/unit-test/roms/Donkey Kong.nes");
  if (*state == NULL)
		return -1;
	return 0;
}

static void test_loadROM_NROM(void **state){
  Mapper * mapper = (Mapper*)*state;
  assert_int_equal(mapper->get,MapNROM_Get);
  assert_int_equal(mapper->destroyer,MapNROM_Destroy);
  assert_int_equal(mapper->ack,MapNROM_Ack);
  uint8_t * romAddr = mapper->get(mapper->mapperData,AS_LDR,LDR_PRG);
  for(int i=0 ; i<32768; i++){
    assert_int_equal(*(romAddr+i),prg[i]);
  }
  uint8_t * vromAddr = mapper->get(mapper->mapperData,AS_LDR,LDR_CHR);
  for(int i=0 ; i<8192; i++){
    assert_int_equal(*(vromAddr+i),chr[i]);
  }
}

static int teardown_loadROM_NROM(void **state){
  Mapper * mapper = (Mapper*)*state;
  MapNROM_Destroy(mapper->mapperData);
  free(mapper);
  if(*state != NULL){
    return 0;
  } else {
    return -1;
  }
}

int run_UTloader(void){
  const struct CMUnitTest test_LOADROM[] = {
    cmocka_unit_test(test_loadROM_path),
    cmocka_unit_test(test_loadROM_size),
    cmocka_unit_test(test_loadROM_format),
    cmocka_unit_test(test_loadROM_rip),
    cmocka_unit_test(test_loadROM_mapperNotDescribed),
  };
  const struct CMUnitTest test_HEADER[] = {
    cmocka_unit_test(test_fillHeader),
  };
  const struct CMUnitTest test_NROM_LOADING[] = {
    cmocka_unit_test(test_loadROM_NROM),
  };
  int out = 0;
  out += cmocka_run_group_tests(test_LOADROM, NULL, NULL);
  out += cmocka_run_group_tests(test_HEADER, setup_fillHeader, teardown_fillHeader);
  out += cmocka_run_group_tests(test_NROM_LOADING, setup_loadROM_NROM, teardown_loadROM_NROM);
  return out;
}

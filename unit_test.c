/* CMocka library */
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "src/nes/mapper/nrom.h"

static int setup_NROM_16(void **state) {
	*state = (void *) MapNROM_Create(NROM_16KIB, NROM_HORIZONTAL);
	if (*state == NULL)
		return -1;
	return 0;
}

static int setup_NROM_32(void **state) {
	*state = (void *) MapNROM_Create(NROM_32KIB, NROM_HORIZONTAL);
	if (*state == NULL)
		return -1;
	return 0;
}

static void test_MapNROM_Get(void **state) {
	MapNROM *self = (MapNROM*) *state; 
	uint8_t *ptr = NULL;
	uint16_t i = 0;

	/* Test CPU RAM memory space */
	for (i = 0; i < 0x2000; i++) {
		ptr = MapNROM_Get(self, AS_CPU, i);
		assert_ptr_equal((void*) ptr, (void*) (self->cpu.ram + (i % 0x800)));
	}

	/* Test CPU IO bank 1 memory space */
	for (i = 0x2000; i < 0x4000; i++) {
		ptr = MapNROM_Get(self, AS_CPU, i);
		assert_ptr_equal((void*) ptr, 
				(void*) (self->cpu.ioReg->bank1 + (i % 8)));
	}

	/* Test CPU IO bank 2 memory space */
	for (i = 0x4000; i < 0x4020; i++) {
		ptr = MapNROM_Get(self, AS_CPU, i);
		assert_ptr_equal((void*) ptr, 
				(void*) (self->cpu.ioReg->bank2 + (i & 0x00FF)));
	}

	/* Test CPU SRAM memory space */
	for (i = 0x6000; i < 0x7FFF; i++) {
		ptr = MapNROM_Get(self, AS_CPU, i);
		assert_ptr_equal((void*) ptr, 
				(void*) (self->cpu.sram + i % 0x2000));
	}

	/* Test CPU ROM memory space */
	uint16_t mod = (self->romSize == NROM_32KIB) ? 0x8000 : 0x4000;
	for (i = 0x8000; i < 0xFFFF; i++) {
		ptr = MapNROM_Get(self, AS_CPU, i);
		assert_ptr_equal((void*) ptr, (void*) (self->cpu.rom + (i % mod)));
	}
}

static void test_IOReg_AckNoRead(void **state) {
	uint16_t i;
	for (i = 0x2000; i < 0x4020; i++)
		assert_int_equal(0, MapNROM_Ack(*state, i)); 
}

static void test_IOReg_AckIsRead(void **state) {
	uint16_t i;
	for (i = 0x3FF8; i < 0x4020; i++)
		assert_int_equal(1, MapNROM_Ack(*state, i)); 
}

static int teardown(void **state) {
	if (*state != NULL) {
		MapNROM_Destroy(*state);
		return 0;
	} else
		return -1;
}

const struct CMUnitTest test_nrom[] = {
	cmocka_unit_test(test_IOReg_AckNoRead),
	cmocka_unit_test(test_MapNROM_Get),
	cmocka_unit_test(test_IOReg_AckIsRead),
	cmocka_unit_test(test_IOReg_AckNoRead),
};

int main() {
	cmocka_run_group_tests(test_nrom, setup_NROM_16, teardown);
	return cmocka_run_group_tests(test_nrom, setup_NROM_32, teardown);
}


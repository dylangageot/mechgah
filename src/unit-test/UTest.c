#include "UTest.h"

int main(void) {
	int out = 0;
	out += run_UTioreg();
	out += run_UTnrom();
	out += run_instruction();
	out += run_UTloader();
	out += run_UTcpu();
	out += run_UTstack();
	out += run_UTppu();
	out += run_UTjoypad();
	out += run_UTcontroller();
	out += run_UTkeys();
	out += run_UTnes();
	return out;
}

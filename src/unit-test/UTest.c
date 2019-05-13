#include "UTest.h"

int main(void) {
	int out = 0;
	out += run_UTnrom();
	out += run_instruction();
	out += run_UTloader();
	out += run_UTcpu();
	out += run_UTstack();
	out += run_UTppu();
	out += run_UTkeys();
	return out;
}

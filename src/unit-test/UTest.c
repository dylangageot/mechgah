#include "UTest.h"

int main(void) {
	int out = 0;
	out += run_UTnrom();
	out += run_instruction();
	out += run_UTloader();
	return out;
}

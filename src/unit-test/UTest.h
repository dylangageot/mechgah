/* CMocka library */
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

/**
 * \fn run_UTnrom
 * \brief Unit test of NROM mapper
 *
 * \return 0 if passed, number of failed otherwise
 */
int run_UTnrom(void);


/**
 * \fn run_UTinstruction
 * \brief Unit test of instructions
 *
 * \return 0 if passed, number of failed otherwise
 */
int run_instruction(void);

/**
 * \fn run_UTloader
 * \brief Unit test of loader
 *
 * \return 0 if passed, number of failed otherwise
 */
int run_UTloader(void);

/**
 * \fn run_UTcpu
 * \brief Unit test of CPU module
 *
 * \return 0 if passed, number of failed otherwise
 */
int run_UTcpu(void);

/**
 * \fn run_UTstack
 * \brief Unit test of Stack module
 *
 * \return 0 if passed, number of failed otherwise
 */
int run_UTstack(void);

/**
 * \fn run_UTppu
 * \brief Unit test of PPU module
 *
 * \return 0 if passed, number of failed otherwise
 */
int run_UTppu(void);

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
 * \fn run_UTinterrupt
 * \brief Unit test of interrupt manager
 *
 * \return 0 if passed, number of failed otherwise
 */
int run_UTinterrupt(void);

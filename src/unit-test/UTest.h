/* CMocka library */
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

/**
 * \brief Unit test of NROM mapper
 *
 * \return 0 if passed, number of failed otherwise
 */
int run_UTnrom(void);

/**
 * \brief Unit test of instructions
 *
 * \return 0 if passed, number of failed otherwise
 */
int run_instruction(void);

/**
 * \brief Unit test of loader
 *
 * \return 0 if passed, number of failed otherwise
 */
int run_UTloader(void);

/**
 * \brief Unit test of CPU module
 *
 * \return 0 if passed, number of failed otherwise
 */
int run_UTcpu(void);

/**
 * \brief Unit test of Stack module
 *
 * \return 0 if passed, number of failed otherwise
 */
int run_UTstack(void);

/**
 * \brief Unit test of PPU module
 *
 * \return 0 if passed, number of failed otherwise
 */
int run_UTppu(void);

/**
<<<<<<< HEAD
 * \brief Unit test of IOReg module
 *
 * \return 0 if passed, number of failed otherwise
 */
int run_UTioreg(void);

/**
 * \brief Unit test of Joypad module
 *
 * \return 0 if passed, number of failed otherwise
 */
int run_UTjoypad(void);

/**
 * \brief Unit test of Controller module
 *
 * \return 0 if passed, number of failed otherwise
 */
int run_UTcontroller(void);
 
/**
 * \brief Unit test of keys module
 *
 * \return 0 if passed, number of failed otherwise
 */
int run_UTkeys(void);

/**
 * \brief Unit test of NES module
 *
 * \return 0 if passed, number of failed otherwise
 */
int run_UTnes(void);

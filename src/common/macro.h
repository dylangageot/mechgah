/**
 * \file macro.h
 * \brief header file of commonly used macro
 * \author Dylan Gageot
 * \version 1.0
 * \date 2019-05-11
 */

#ifndef MACRO_H
#define MACRO_H

/**
 * \brief Error printed to stderr 
 */
#define ERROR_MSG(str) fprintf(stderr, "Error: %s at %s, line %d.\n", str, \
						__FILE__, __LINE__);

/**
 * \brief Test if x is between specified values (y and z)
 */
#define VALUE_IN(x,y,z)	(((x) >= (y)) && ((x) <= (z)))

/**
 * \brief Test if x is superior to y
 */
#define VALUE_SUP(x,y)	(((x) >= (y)))

/**
 * \brief Test if x is inferior to y
 */
#define VALUE_INF(x,y)	(((x) <= (y)))

#endif /* MACRO_H */

#ifndef MACRO_H
#define MACRO_H

/**
 * \brief Error printed to stderr 
 */
#define ERROR_MSG(str) fprintf(stderr, "Error: %s at %s, line %d.\n", str, \
						__FILE__, __LINE__);

/**
 * \brief Useful macro when dealing with address mapping
 */
#define ADDRESS_IN(x,y,z)	(((x) >= (y)) && ((x) <= (z)))
#define ADDRESS_SUP(x,y)	(((x) >= (y)))
#define ADDRESS_INF(x,y)	(((x) <= (y)))

#endif /* MACRO_H */

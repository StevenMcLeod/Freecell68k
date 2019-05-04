#ifndef STRNTOL_H
#define STRNTOL_H

#include <stddef.h>

long strntol(const char *nptr, size_t sz, char **endptr, int base);

#endif /* STRNTOL_H */
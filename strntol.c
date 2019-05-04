/*
 *	Adapted from GCC strtol.c
 */
 
#include "strntol.h"
#include <limits.h>
#include <ctype.h>
 
long strntol(const char *nptr, size_t sz, char **endptr, int base) {
	const char *s = nptr;
	unsigned long acc = 0;
	int c;
	unsigned long cutoff;
	int neg = 0, any = 0, cutlim;
	
	++sz;	/* Bounds fix */

	/* Skip whitespace, pick up +/- sign, detect prefix. */
	do {
		c = *s++;
		--sz;
	} while(isspace(c) && sz > 0);
	if(sz == 0) {
		goto err;
	}
	
	if(c == '-') {
		neg = 1;
		c = *s++;
		sz--;
	} else if(c == '+') {
		c = *s++;
	}
	
	
	if((base == 0 || base == 16) &&
			sz >= 2 && c == '0' && (*s == 'x' || *s == 'X')) {
		c = s[1];
		s += 2;
		sz -= 2;
		base = 16;
	}
	
	if(base == 0) {
		base = c == '0' ? 8 : 10;
	}
	
	/* Compute cutoff between legal / illegal numbers */
	cutoff = neg ? -(unsigned long) LONG_MIN : LONG_MAX;
	cutlim = cutoff % (unsigned long) base;
	cutoff /= (unsigned long) base;
	for(;; --sz, c = *s++) {
		if(sz == 0) {
			break;
		}
		
		if(isdigit(c)) {
			c -= '0';
		} else if(isalpha(c)) {
			c -= isupper(c) ? 'A' - 10 : 'a' - 10;
		} else {
			break;
		}
		
		if(c >= base) {
			break;
		}
		
		if(any < 0 || acc > cutoff || (acc == cutoff && c > cutlim)) {
			any = -1;
		} else {
			any = 1;
			acc *= base;
			acc += c;
		}
	}
	
	if(any < 0) {
		acc = neg ? LONG_MIN : LONG_MAX;
	} else if(neg) {
		acc = -acc;
	}
	
err:
	if(endptr != 0) {
		*endptr = (char *) (any ? s - 1 : nptr);
	}
	
	return acc;
}
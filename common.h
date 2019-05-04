#ifndef COMMON_H
#define COMMON_H

typedef unsigned short ushort;
typedef unsigned long ulong;

struct PSTRUCT {
	Byte len;
	Byte str[];
};

#define ABS(x) ((x)>0?(x):-(x))

#define HIWORD(x) (((x) & 0xFFFF0000) >> 16)
#define LOWORD(x) ((x) & 0x0000FFFF)

#define TO_PSTRUCT(x) (* (struct PSTRUCT *) (x))

#define PLEN(x) (TO_PSTRUCT(x).len)
#define PSTR(x) (TO_PSTRUCT(x).str)

#ifndef M_PI
#define M_PI 3.1415926535897932
#endif

#define ITOC(n) ((n) + '0')
#define CTOI(n) ((n) - '0')

#define ARRLEN(a) (sizeof(a)/sizeof(*(a)))

#define L2PT(p,n) 	do { (p).v = HIWORD(n); (p).h = LOWORD(n); } while(0)
#define PT2L(p) 	(((long)(p).v << 16) | (long) (p).h)
#define TO_PTL(v,h) (((long)LOWORD(v) << 16) | (long) LOWORD(h))

#endif
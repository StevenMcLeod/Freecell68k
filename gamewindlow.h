#ifndef GAMEWINDLOW_H
#define GAMEWINDLOW_H

#include "common.h"
#include "freecell.h"

/* Game Dimensions */

#define CARDFONT_ID 25
#define CARDFONT_PT 12

#define CARD_XRATIO 5
#define CARD_YRATIO 7
#define CARD_SCALE 9

#define CARD_XLENGTH (CARD_XRATIO * CARD_SCALE)
#define CARD_YLENGTH (CARD_YRATIO * CARD_SCALE)

/*
	^
	|BD_Y
	|
BD_Xv    ST_X           ST_GAP
<-->+--+<---->+--+ ... <------>
	|  |      |  |
	|  |      |  |
	+--+      +--+
    ^
  ST|
 SEP|			XLEN
	v     PF_X <-->
	+--+<----->+--+^
PF_Y|  |       |  ||YLEN
	+--+       |  ||
	|  |       +--+v
	...
*/
/* Set to a ratio ? */
#define CARD_PF_X 	10
#define CARD_PF_Y 	12

#define CARD_ST_X	((WIND_XLENGTH - (2*CARD_BD_X) - (8*CARD_XLENGTH) \
					- CARD_ST_GAP) / 6)
#define CARD_ST_GAP	40
#define CARD_ST_SEP (2*CARD_BD_Y)

#define CARD_BD_X	10
#define CARD_BD_Y	10

#define WIND_XLENGTH ((2*CARD_BD_X)+(8*CARD_XLENGTH)+(7*CARD_PF_X))
#define WIND_YLENGTH \
	(((long) (screenBits.bounds.bottom-20) * WIND_XLENGTH) / screenBits.bounds.right)

#define SUIT2CHAR(s) ('0' - 1 - (s))

#define INVAL_LOC (0x8000)
#define INVAL_PTL (0x80008000L)
#define BAD_PT(p) (((p).h == INVAL_LOC) && ((p).v == INVAL_LOC))
/*#define BAD_PT(p) BAD_PTL(PT2L(p))*/
#define BAD_PTL(p) (p == INVAL_PTL)

/* Low-Level Drawing */
void DrawCard(Card card, Point loc);
void DrawStackedCard(Card card, Point loc, ushort pos);
void DrawStack(Card cards[], Point loc, ushort qty);
void DrawEmptyFrame(Point loc);

/* Low-Level Point Access */
Point GetStorePt(ushort store);
Point GetColumnPt(ushort col);
void GetStoreRect(ushort store, Rect *res);
void GetColumnRect(ushort col, ushort csize, Rect *res);
void GetStackedCardRect(ushort col, ushort csize, ushort pos, Rect *res);
void GetLastStackedRect(ushort col, ushort csize, Rect *res);


#endif /* GAMEWINDLOW_H */
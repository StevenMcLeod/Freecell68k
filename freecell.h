#ifndef FREECELL_H
#define FREECELL_H

#include "common.h"
#include "gametypes.h"

#define FC_STORES 4
#define FC_COLS 8
#define FC_DCSIZE 13

/* Seed values [1, 32001) */
#define FC_SEEDLO 	1
#define FC_SEEDHI 	32001

/* GameElem Structs */
enum {
	FCZ_NONE=0,
	FCZ_LASTCARD,
	FCZ_COLUMN,
	FCZ_STORAGE,
	FCZ_FOUNDATION
};

typedef struct {
	Byte zone;
	Byte num;
} FCZone;

#define FCZONE_EQ(a, b) (\
	((a).zone == (b).zone) \
	&& ((a).num == (b).num) \
)

#define SIZEOF_FCCOLUMN(qty) (sizeof(FCColumn) + (qty) * sizeof(Card))

typedef struct {
	ushort qty;
	Card cards[];
} FCColumn;

typedef struct {
	ushort 		moves;
	ushort 		seedno;
	FCColumn 	**cols;				/* Size FC_COLS */
	Card 		store[FC_STORES];
	Card 		foundation[4];
	FCZone 		lastMove[2];
} FCState;

/* Game Setup */
void FreecellInit(FCState *state);
void FreecellStartGame(FCState *state, ushort seed);
FCColumn **FreecellShuffle(ushort seed, FCColumn **deal);
void FreecellDisposeDeal(FCColumn **deal);

/* Game Play */
Card GetCardAt(FCState *state, FCZone elem);
Boolean FreecellLegalMove(FCState *state, FCZone from, FCZone to);
Boolean FreecellPlayMove(FCState *state, FCZone from, FCZone to);
void FreecellForceMove(FCState *state, FCZone from, FCZone to);
void FreecellUndoMove(FCState *state);

#endif /* FREECELL_H */
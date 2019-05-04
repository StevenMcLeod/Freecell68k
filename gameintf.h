#ifndef GAMEINTF_H
#define GAMEINTF_H

#include "freecell.h"

#define GAME_RANDOMSEED	0

typedef enum {
	FCCE_OK=0,
	FCCE_BADHIT,
	FCCE_NODRAG,
	FCCE_OOBDRAG,
	FCCE_BADDRAG,
	FCCE_BADMOVE
} FCClickErr;

/* Mouse and Gameplay Handlers */
FCClickErr HandleGameClick(FCState *state, Point hitPt);
void GameDrawMove(FCState *state, FCZone from, FCZone to);

void GameNewGame(FCState *state, ushort seed);

FCZone GetGamePtLoc(FCState *state, Point pt, Rect *elemRect);
void GetFCZoneRect(FCState *state, FCZone elem, Rect *elemRect);

#endif /* GAMEINTF_H */
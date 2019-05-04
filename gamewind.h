#ifndef GAMEWIND_H
#define GAMEWIND_H

#include "common.h"
#include "gametypes.h"
#include "freecell.h"

/* Window / Dialog creation */
WindowPtr WindCreateTestEnv(Rect *bounds, StringPtr title);
WindowPtr WindCreateGame(FCState *state);
void WindUpdateTitle(WindowPtr theWind);

ushort DlogOpenGame(void);
void DlogAbout(void);

/* High-Level Drawing */
void DrawGameInit(void);
void ForceRedraw(void);
void DrawAll(FCState *state);
void DrawClear(void);
void DrawPlayfield(FCColumn **cols);
void DrawStorage(Card *cards);
void DrawFoundation(Card *cards);

void DrawStorageCard(Card card, ushort pos);
void DrawFoundationCard(Card card, ushort pos);
void DrawLastColumnCard(FCColumn *col, ushort colno);
void DrawRemovedColCard(FCColumn *col, ushort colno);

/* Misc */
void CentreRect(Rect *toCentre, const Rect *bounds);

#endif /* GAMEWIND_H */
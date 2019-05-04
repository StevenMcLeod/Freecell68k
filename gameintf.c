#include "gameintf.h"
#include "gamewind.h"
#include "gamewindlow.h"
#include "gamemenu.h"
#include "freecell.h"

/* HandleGameClick, DragActionProc */
static FCZone startHoverElem;
static FCZone lastHoverElem;
static Boolean lastHoverLegal;

static pascal void DragActionProc(void);

static short RndRange(short lower, short upper);

FCClickErr HandleGameClick(FCState *state, Point hitPt) {
	FCZone 		hitElem, dragElem;
	Point 		dragPt;
	long 		theLPoint;
	Rect 		boundsRect = {-3, -3, 3, 3};
	Rect 		cardRect;
	RgnHandle 	cardRgn;
	FCClickErr	theErr;
	
	hitElem = GetGamePtLoc(state, hitPt, &cardRect);
	if(hitElem.zone == FCZ_NONE
			|| CARD_EMPTY(GetCardAt(state, hitElem))) {
		theErr = FCCE_BADHIT;
		goto err;
	}
	
	/* Drag has slack to differentiate from double-click */
	dragPt = hitPt;
	OffsetRect(&boundsRect, dragPt.h, dragPt.v);
	while(PtInRect(dragPt, &boundsRect) && StillDown()) {
		GetMouse(&dragPt);
	}
	
	if(!StillDown()) {
		theErr = FCCE_NODRAG;
		goto err;
	}
	
	/* Rect now starts in a different location */
	OffsetRect(&cardRect, dragPt.h - hitPt.h, dragPt.v - hitPt.v);
	cardRgn = NewRgn();
	OpenRgn();
	FrameRoundRect(&cardRect, CARD_XRATIO, CARD_YRATIO);
	CloseRgn(cardRgn);
	
	/* Create drag outline */
	boundsRect = FrontWindow()->portRect;
	startHoverElem = hitElem;
	lastHoverElem.zone = FCZ_NONE;
	lastHoverLegal = false;
	theLPoint = DragGrayRgn(cardRgn, dragPt, &boundsRect, 
			&boundsRect, noConstraint, &DragActionProc);
	if(BAD_PTL(theLPoint)) {
		theErr = FCCE_OOBDRAG;
		goto err;
	}
		
	dragPt.v += HIWORD(theLPoint);
	dragPt.h += LOWORD(theLPoint);
	dragElem = GetGamePtLoc(state, dragPt, &cardRect);
	if(dragElem.zone == FCZ_NONE) {
		theErr = FCCE_BADDRAG;
		goto err;
	}
	
	if(!lastHoverLegal) {
		theErr = FCCE_BADMOVE;
		goto err;
	}
	
	InvertRoundRect(&cardRect, CARD_XRATIO, CARD_YRATIO);
	/* Play move */
	if(!FreecellPlayMove(state, hitElem, dragElem)) {
		theErr = FCCE_BADMOVE;
		goto err;
	}
	
	GameDrawMove(state, hitElem, dragElem);
	return FCCE_OK;

err:
	/* In case other deinit needs to occur */
	return theErr;
}

pascal void DragActionProc(void) {
	Point 		mousePoint;
	FCZone 		hoverElem;
	Rect		hoverRect, unhoverRect;
	FCState		*state;
	
	/* Invert spaces that are able to be placed */
	GetMouse(&mousePoint);
	state = (FCState *) GetWRefCon(FrontWindow());
	hoverElem = GetGamePtLoc(state, mousePoint, &hoverRect);
	if(FCZONE_EQ(hoverElem, lastHoverElem))
		return;
	
	if(lastHoverLegal) {
		GetFCZoneRect(state, lastHoverElem, &unhoverRect);
		InvertRoundRect(&unhoverRect, CARD_XRATIO, CARD_YRATIO);
	}
	
	lastHoverElem = hoverElem;
	lastHoverLegal = FreecellLegalMove(state, startHoverElem, hoverElem);
	if(lastHoverLegal) {
		InvertRoundRect(&hoverRect, CARD_XRATIO, CARD_YRATIO);
	}
}

void GameDrawMove(FCState *state, FCZone from, FCZone to) {
	Card replaceCard = GetCardAt(state, from);
	Card moveCard = GetCardAt(state, to);
	
	switch(from.zone) {
	case FCZ_LASTCARD:
		DrawRemovedColCard(state->cols[from.num], from.num);
		break;
	
	case FCZ_STORAGE:
		DrawStorageCard(replaceCard, from.num);
		break;
		
	case FCZ_FOUNDATION:
		DrawFoundationCard(replaceCard, from.num);
		break;
	}
	
	switch(to.zone) {
	case FCZ_LASTCARD:
		DrawLastColumnCard(state->cols[to.num], to.num);
		break;
	
	case FCZ_STORAGE:
		DrawStorageCard(moveCard, to.num);
		break;
		
	case FCZ_FOUNDATION:
		DrawFoundationCard(moveCard, to.num);
		break;
	}
	
	if(state->moves <= 1) {
		MenuUndoState(true);
	}
}

void GameNewGame(FCState *state, ushort seed) {
	Boolean needTitleUpdate;
	if(!seed) {
		seed = RndRange(FC_SEEDLO, FC_SEEDHI);
	}
	
	needTitleUpdate = seed != state->seedno;
	FreecellStartGame(state, seed);
	if(needTitleUpdate) {
		WindUpdateTitle(FrontWindow());
	}
	MenuUndoState(false);
	ForceRedraw();
}

/* To optimize */
/* elemRect can be NULL if not required */
FCZone GetGamePtLoc(FCState *state, Point pt, Rect *elemRect) {
	FCZone res;
	Rect boundsRect;
	ushort i;
	
	for(i = 0; i < FC_STORES + 4; ++i) {
		GetStoreRect(i, &boundsRect);
		if(PtInRect(pt, &boundsRect)) {
			res.zone = (i >= FC_STORES) ? FCZ_FOUNDATION : FCZ_STORAGE;
			res.num = (i >= FC_STORES) ? i - FC_STORES : i;
			if(elemRect) *elemRect = boundsRect;
			return res;
		}
	}
	
	for(i = 0; i < FC_COLS; ++i) {
		GetColumnRect(i, state->cols[i]->qty, &boundsRect);
		if(PtInRect(pt, &boundsRect)) {
			if(elemRect) *elemRect = boundsRect;
			GetLastStackedRect(i, state->cols[i]->qty, &boundsRect);
			if(PtInRect(pt, &boundsRect)) {
				res.zone = FCZ_LASTCARD;
				if(elemRect) *elemRect = boundsRect;
			} else {
				res.zone = FCZ_COLUMN;
			}
			
			res.num = i;
			return res;
		}
	}
	
	res.zone = FCZ_NONE;
	if(elemRect)
		SetRect(elemRect, 0, 0, 0, 0);
	return res;
}

void GetFCZoneRect(FCState *state, FCZone elem, Rect *elemRect) {
	switch(elem.zone) {
	case FCZ_LASTCARD: {
		FCColumn *theColumn = state->cols[elem.num];
		GetLastStackedRect(elem.num, theColumn->qty, elemRect);
	} 	break;
	case FCZ_COLUMN: {
		FCColumn *theColumn = state->cols[elem.num];
		GetColumnRect(elem.num, theColumn->qty, elemRect);
	}	break;
			
	case FCZ_STORAGE:
		GetStoreRect(elem.num, elemRect);
		break;
		
	case FCZ_FOUNDATION:
		GetStoreRect(elem.num + FC_STORES, elemRect);
		break;
	
	default:
		SetRect(elemRect, 0, 0, 0, 0);
	}
}

/* If upper <= lower then result undefined. */
short RndRange(short lower, short upper) {
	asm {
		CLR.W	-(sp)		;  d0 = Random()
		_Random
		CLR.L	d0
		MOVE.W	(sp)+, d0
		MOVE.W	upper, d1	;  d1 = upper - lower
		SUB.W	lower, d1
		DIVU.W	d1, d0		;  d0 = d0 % d1
		SWAP	d0
		ADD.W	lower, d0
	}
}
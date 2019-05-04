#include "gamewindlow.h"

void DrawCard(Card card, Point loc) {
	Rect cardRect;
	static short lineAscent = -1;
	static short lineSpacing = -1;
	
	SetRect(&cardRect, 0, 0, CARD_XLENGTH, CARD_YLENGTH);
	OffsetRect(&cardRect, loc.h, loc.v);
	EraseRoundRect(&cardRect, CARD_XRATIO, CARD_YRATIO);
	if(CARD_EMPTY(card)) return;
	
	TextFont(CARDFONT_ID);
	TextSize(9);
	if(lineSpacing == -1) {
		FontInfo cardFontInfo;
		GetFontInfo(&cardFontInfo);
		lineAscent = cardFontInfo.ascent;
		lineSpacing = cardFontInfo.ascent +
						cardFontInfo.descent +
						cardFontInfo.leading;
	}
	
	FillRoundRect(&cardRect, CARD_XRATIO, CARD_YRATIO, white);
	FrameRoundRect(&cardRect, CARD_XRATIO, CARD_YRATIO);
	InsetRect(&cardRect, (CARD_XLENGTH * 9) / 40,
			(CARD_YLENGTH * 9) / 40);
	FillRoundRect(&cardRect, CARD_XRATIO, CARD_YRATIO, gray);
	
	loc.h += (CARD_XLENGTH * 2) / 40;
	loc.v += (CARD_YLENGTH * 2) / 40 + lineAscent;
	MoveTo(loc.h, loc.v);
	DrawChar(ITOC(CARD_GETNUM(card)));
	/*MoveTo(loc.h, loc.v + lineSpacing);*/
	DrawChar(SUIT2CHAR(CARD_GETSUIT(card)));
}

void DrawStack(Card cards[], Point loc, ushort qty) {
	ushort i;
	
	if(qty == 0) {
		DrawEmptyFrame(loc);
		return;
	}
	
	for(i = 0; i < qty; ++i) {
		DrawCard(cards[i], loc);
		loc.v += CARD_PF_Y;
	}
}

void DrawStackedCard(Card card, Point loc, ushort pos) {
	loc.v += CARD_PF_Y * pos;
	DrawCard(card, loc);
}

void DrawEmptyFrame(Point loc) {
	PenState ps;
	Rect frameRect = {0, 0, CARD_YLENGTH, CARD_XLENGTH};
	
	GetPenState(&ps);
	PenPat(black);
	PenSize(1,1);
	OffsetRect(&frameRect, loc.h, loc.v);
	FillRoundRect(&frameRect, CARD_XRATIO, CARD_YRATIO, white);
	FrameRoundRect(&frameRect, CARD_XRATIO, CARD_YRATIO);
	SetPenState(&ps);
}

/* Returns (INVAL_LOC, INVAL_LOC) on error */
Point GetStorePt(ushort store) {
	Point res;
	if(store >= FC_STORES + 4) {
		L2PT(res, INVAL_PTL);
		return res;
	}
	
	if(store >= FC_STORES) {
		/* Set count from 4-7 to 1-4 backwards */
		/*store = (4 - 1) - (store - FC_STORES) + 1;*/
		store = (FC_STORES + 4) - store;
		res.h = WIND_XLENGTH - CARD_BD_X + CARD_ST_X /* Correction Term */
			- store * (CARD_XLENGTH + CARD_ST_X);
	} else {
		res.h = CARD_BD_X + store * (CARD_XLENGTH + CARD_ST_X);
	}
	
	res.v = CARD_BD_Y;
	return res;
}

Point GetColumnPt(ushort col) {
	Point res;
	if(col >= FC_COLS) {
		L2PT(res, INVAL_PTL);
		return res;
	}
	
	res.v = CARD_BD_Y + CARD_YLENGTH + CARD_ST_SEP;
	res.h = CARD_BD_X + col * (CARD_XLENGTH + CARD_PF_X);
	return res;
}

void GetStoreRect(ushort store, Rect *res) {
	topLeft(*res) = GetStorePt(store);
	if(BAD_PT(topLeft(*res))) {
		SetRect(res, 0, 0, 0, 0);
		return;
	}
	
	res->bottom = res->top + CARD_YLENGTH;
	res->right = res->left + CARD_XLENGTH;
}

void GetColumnRect(ushort col, ushort csize, Rect *res) {
	topLeft(*res) = GetColumnPt(col);
	if(BAD_PT(topLeft(*res))) {
		SetRect(res, 0, 0, 0, 0);
		return;
	}
	
	if(csize == 0) csize = 1;
	res->bottom = res->top + (csize - 1) * CARD_PF_Y + CARD_YLENGTH;
	res->right = res->left + CARD_XLENGTH;
}

/* 0 treated as 1 so that cards can be played on empty columns */
void GetStackedCardRect(ushort col, ushort csize, ushort pos, Rect *res) {
	if(csize == 0) csize = 1;
	
	if(pos >= csize)
		goto err;
	
	topLeft(*res) = GetColumnPt(col);
	if(BAD_PT(topLeft(*res)))
		goto err;
		
	res->top += pos * CARD_PF_Y;
	res->right = res->left + CARD_XLENGTH;
	
	/* If card is last in column, use full area. Otherwise whats seen. */
	if(pos == csize - 1) {
		res->bottom = res->top + CARD_YLENGTH;
	} else {
		res->bottom = res->top + CARD_PF_Y;
	}
	
	return;
err:
	SetRect(res, 0, 0, 0, 0);	
}

void GetLastStackedRect(ushort col, ushort csize, Rect *res) {
	if(csize < 1) csize = 1;
	GetStackedCardRect(col, csize, csize-1, res);
}
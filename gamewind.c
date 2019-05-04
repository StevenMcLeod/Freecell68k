#include "gamewind.h"
#include "gamewindlow.h"
#include "common.h"
#include "pstring.h"
#include "strntol.h"
#include <string.h>
#include <ctype.h>

#define TITLE_PREF 		"\pFreeCell - #"
#define DLOG_OPEN		128
#define DLOG_OPEN_INPUT 3
#define DLOG_ABOUT		129

typedef struct {
	short 	dlgMaxIndex;
	Handle 	itmHndl;
	Rect	itmRect;
	short	itmType;
	Byte	itmData[];
} DialogItemList;

static pascal Boolean DigitInputFilter(DialogPtr theDialog,
		EventRecord *theEvent, short *itemHit);
static pascal Boolean AboutFilter(DialogPtr theDialog,
		EventRecord *theEvent, int *itemHit);

WindowPtr WindCreateTestEnv(Rect *bounds, StringPtr title) {
	WindowPtr 	theWind;
	Rect		windRect;
	
	title = title ? title : "\pTest Area";
	theWind = NewWindow(0L, bounds, title, true,
		noGrowDocProc, (WindowPtr) -1L, true, 0);
		
	return theWind;
}

WindowPtr WindCreateGame(FCState *state) {
	WindowPtr 		theWind;
	Rect			boundsRect;
	Rect			windRect;
	StringPtr		titleString;
	unsigned char 	gameNumBuf[6];
	Pattern			bkpat;
	/*
	titleString = (StringPtr) NewPtr(sizeof(Str255));
	if(!titleString) return NULL;
	memcpy(titleString, TITLE_PREF, sizeof(TITLE_PREF));
	NumToString(state->seedno, gameNumBuf);
	strcat_p(titleString, gameNumBuf);*/
	
	SetRect(&windRect, 0, 0, WIND_XLENGTH, WIND_YLENGTH);
	boundsRect = screenBits.bounds;
	boundsRect.top += 20;
	CentreRect(&windRect, &boundsRect);
	theWind = NewWindow(0L, &windRect, "\p", true, noGrowDocProc,
		(WindowPtr) -1L, true, 0);
	/*DisposPtr(titleString);*/
	if(theWind) {
		SetPort(theWind);
		GetIndPattern(&bkpat, sysPatListID, 21);
		BackPat(bkpat);
		SetWRefCon(theWind, (long) state);
	}
	return theWind;
}

void WindUpdateTitle(WindowPtr theWind) {
	StringPtr		titleString;
	unsigned char 	gameNumBuf[6];
	FCState			*state = (FCState *) GetWRefCon(theWind);
	
	titleString = (StringPtr) NewPtr(sizeof(Str255));
	if(!titleString) return;
	memcpy(titleString, TITLE_PREF, sizeof(TITLE_PREF));
	NumToString(state->seedno, gameNumBuf);
	strcat_p(titleString, gameNumBuf);
	SetWTitle(theWind, titleString);
	DisposPtr(titleString);
}

ushort DlogOpenGame(void) {
	DialogPtr		theDialog;
	Rect			dlogRect, boundsRect;
	short 			itemNo;
	Handle			textboxHandle;
	volatile Size	textboxHndlSize;
	long			inputText;
	
	theDialog = GetNewDialog(DLOG_OPEN, NULL, (WindowPtr) -1);
	if(!theDialog) return -1;
	dlogRect = theDialog->portRect;
	boundsRect = screenBits.bounds;
	boundsRect.top += 20;
	CentreRect(&dlogRect, &boundsRect);
	MoveWindow(theDialog, dlogRect.left, dlogRect.top, true);
	ShowWindow(theDialog);
	
	do {
		do {
			ModalDialog(&DigitInputFilter, &itemNo);
			if(itemNo == cancel) {
				inputText = -1;
				goto err;
			}
		} while(itemNo != ok);
		
		GetDItem(theDialog, DLOG_OPEN_INPUT, &itemNo, 
				&textboxHandle, &boundsRect);
		textboxHndlSize = GetHandleSize(textboxHandle);
		/* Ensure whitespace before / after string */
		HLock(textboxHandle);
		inputText = strntol(*textboxHandle, textboxHndlSize, NULL, 10);
		HUnlock(textboxHandle);
		if(inputText < 1 || inputText > 32000) {
			SysBeep(1);
		} else {
			break;
		}
	} while(true);
	
err:
	DisposDialog(theDialog);
	return (ushort) inputText;
}

pascal Boolean DigitInputFilter(DialogPtr theDialog, EventRecord *theEvent, 
		short *itemHit) {
	char theChar;
	
	if(theEvent->what == keyDown || theEvent->what == autoKey) {
		theChar = theEvent->message & 0xFF;
		if(theChar == '\r' || theChar == '\x03') {
			*itemHit = ok;
			return true;
		} else if(!isdigit(theChar) && theChar != '\b') {
			SysBeep(1);
			theEvent->what = nullEvent;
		}
	}
	return false;
}

void DlogAbout(void) {
	DialogPtr	theDialog;
	Rect		dlogRect;
	Rect		boundsRect;
	short		itemNo;

	theDialog = GetNewDialog(DLOG_ABOUT, NULL, (WindowPtr) -1);
	if(!theDialog) return;
	dlogRect = theDialog->portRect;
	boundsRect = screenBits.bounds;
	boundsRect.top += 20;
	boundsRect.bottom /= 2;
	CentreRect(&dlogRect, &boundsRect);
	MoveWindow(theDialog, dlogRect.left, dlogRect.top, true);
	ShowWindow(theDialog);
	ModalDialog(&AboutFilter, &itemNo);
	DisposDialog(theDialog);
}

pascal Boolean AboutFilter(DialogPtr theDialog, EventRecord *theEvent,
		int *itemHit) {
	int 		windowCode;
	WindowPtr 	theWindow;
	
	if(theEvent->what != mouseDown)
		return false;
	windowCode = FindWindow(theEvent->where, &theWindow);
	if(windowCode != inContent || theWindow != theDialog)
		return false;
	
	return true;
}

void DrawGameInit(void) {
	Point drawPoint;
	ushort i;
	
	drawPoint.v = CARD_BD_Y;
	drawPoint.h = CARD_BD_X;
	for(i = 0; i < FC_STORES; ++i) {
		DrawEmptyFrame(drawPoint);
		drawPoint.h += CARD_ST_X + CARD_XLENGTH;
	}
	
	drawPoint.h = WIND_XLENGTH - CARD_BD_X - CARD_XLENGTH;
	for(i = 0; i < 4; ++i) {
		DrawEmptyFrame(drawPoint);
		drawPoint.h -= CARD_ST_X + CARD_XLENGTH;
	}
}

void ForceRedraw(void) {
	GrafPtr thePort;
	GetPort(&thePort);
	InvalRect(&thePort->portRect);
}

void DrawAll(FCState *state) {
	DrawClear();
	DrawStorage(state->store);
	DrawFoundation(state->foundation);
	DrawPlayfield(state->cols);
}

void DrawClear(void) {
	GrafPtr thePort;
	Rect 	*clearRect;
	GetPort(&thePort);
	clearRect = &thePort->portRect;
	EraseRect(clearRect);
}

void DrawPlayfield(FCColumn **cols) {
	Point drawPoint;
	ushort i;
	
	drawPoint.v = CARD_BD_Y + CARD_YLENGTH + CARD_ST_SEP;
	drawPoint.h = CARD_BD_X;
	for(i = 0; i < FC_COLS; ++i) {
		DrawStack(cols[i]->cards, drawPoint, cols[i]->qty);
		drawPoint.h += CARD_XLENGTH + CARD_PF_X;
	}
}

void DrawStorage(Card *cards) {
	Point drawPoint;
	ushort i;
	
	drawPoint.v = CARD_BD_Y;
	drawPoint.h = CARD_BD_X;
	for(i = 0; i < FC_STORES; ++i) {
		if(!CARD_EMPTY(cards[i])) {
			DrawCard(cards[i], drawPoint);
		} else {
			DrawEmptyFrame(drawPoint);
		}
		drawPoint.h += CARD_ST_X + CARD_XLENGTH;
	}
}

void DrawFoundation(Card *cards) {
	Point drawPoint;
	ushort i;
	
	drawPoint.v = CARD_BD_Y;
	drawPoint.h = WIND_XLENGTH - CARD_BD_X - CARD_XLENGTH;
	for(i = 0; i < 4; ++i) {
		if(!CARD_EMPTY(cards[3-i])) {
			DrawCard(cards[3-i], drawPoint);
		} else {
			DrawEmptyFrame(drawPoint);
		}
		drawPoint.h -= CARD_ST_X + CARD_XLENGTH;
	}
}

void DrawStorageCard(Card card, ushort pos) {
	Point drawPoint;
	
	drawPoint = GetStorePt(pos);
	if(!CARD_EMPTY(card)) {
		DrawCard(card, drawPoint);
	} else {
		DrawEmptyFrame(drawPoint);
	}
}

void DrawFoundationCard(Card card, ushort pos) {
	Point drawPoint;
	
	drawPoint = GetStorePt(FC_STORES + pos);
	if(!CARD_EMPTY(card)) {
		DrawCard(card, drawPoint);
	} else {
		DrawEmptyFrame(drawPoint);
	}	
}

void DrawLastColumnCard(FCColumn *col, ushort colno) {
	Point drawPoint;
	
	drawPoint = GetColumnPt(colno);
	DrawStackedCard(col->cards[col->qty-1], drawPoint, col->qty-1);
}
	
void DrawRemovedColCard(FCColumn *col, ushort colno) {
	Point drawPoint;
	
	drawPoint = GetColumnPt(colno);
	DrawStackedCard(CARD_NULL, drawPoint, col->qty);
	
	if(col->qty != 0) {
		DrawStackedCard(col->cards[col->qty-1], drawPoint, col->qty-1);
	} else {
		DrawEmptyFrame(drawPoint);
	}
}

void CentreRect(Rect *toCentre, const Rect *bounds) {
	short bdx, bdy, cdx, cdy;
	
	bdx = bounds->right - bounds->left;
	bdy = bounds->bottom - bounds->top;
	cdx = toCentre->right - toCentre->left;
	cdy = toCentre->bottom - toCentre->top;
	if(cdx > bdx || cdy > bdy) return;
	
	toCentre->top = bounds->top + bdy/2 - cdy/2;
	toCentre->left = bounds->left + bdx/2 - cdx/2;
	toCentre->bottom = toCentre->top + cdy;
	toCentre->right = toCentre->left + cdx;
}

/*
	switch(elem.zone) {
	case FCZ_LASTCARD: {
		FCColumn *theColumn = state->cols[elem.num];
		if(CARD_EMPTY(theColumn->cards[theColumn->qty-1]))
			goto err;
			
		GetLastStackedRect(elem.num, theColumn->qty, &drawRect);
	} 	break;
			
	case FCZ_STORAGE:
		if(CARD_EMPTY(state->store[elem.num]))
			goto err;
			
		GetStoreRect(elem.num, &drawRect);
		break;
		
	case FCZ_FOUNDATION:
		if(CARD_EMPTY(state->foundation[elem.num]))
			goto err;
			
		GetStoreRect(elem.num + FC_STORES, &drawRect);
		break;
	
	default:
		goto err;
	}
	*/
#include "freecell.h"
#include "common.h"
#include <string.h>

#define FC_CEXPAND 2

static void SequenceDeck(Card *deck);
static void AddColumnCard(FCColumn **col, Card card);
static Card GetColumnCard(FCColumn *col);
static Card RemoveColumnCard(FCColumn *col);
static FCColumn *ExpandColumnSize(FCColumn *old, Size ptrSize);

static Boolean AlternatingColours(Card a, Card b);

void FreecellInit(FCState *state) {
	memset(state, 0, sizeof(*state));
}

void FreecellStartGame(FCState *state, ushort seed) {
	state->moves = 0;
	state->seedno = seed;
	state->cols = FreecellShuffle(seed, state->cols);
	memset(state->store, 0, sizeof(state->store));
	memset(state->foundation, 0, sizeof(state->foundation));
}	

FCColumn **FreecellShuffle(ushort seed, FCColumn **deal) {
	ulong state = seed;
	ushort i, j;
	Card deck[CARD_QTY];
	
	/* Allocate deal memory */
	if(!deal) {
		deal = (FCColumn **) NewPtr(FC_COLS * sizeof(FCColumn *));
		if(!deal) return NULL;
		for(i = 0; i < FC_COLS; ++i) {
			deal[i] = (FCColumn *) NewPtr(SIZEOF_FCCOLUMN(FC_DCSIZE));
			if(!deal[i]) goto err_inner;
		}
	}
	
	SequenceDeck(deck);
	
	/* Freecell deal algorithm */
	for(i = 0; i < CARD_QTY - 1; ++i) {
		ushort cidx;
		Card toswap;
		
		state = (state * 214013L + 2531011L) & 0x7FFFFFFFL;
		cidx = CARD_QTY - 1 - ((state >> 16) % (CARD_QTY - i));
		
		toswap = deck[cidx];
		deck[cidx] = deck[i];
		deck[i] = toswap;
	}
	
	for(i = 0; i < FC_COLS; ++i) {
		for(j = 0; 8*j + i < CARD_QTY; ++j) {
			deal[i]->cards[j] = deck[8*j + i];
		}
		
		deal[i]->qty = j;
	}
	
	return deal;
	
err_inner:
	/* Inner deal array error */
	while(i--) {
		DisposPtr(deal[i]);
	}
	DisposPtr(deal);
	return NULL;
}

void FreecellDisposeDeal(FCColumn **deal) {
	ushort i;
	
	if(deal) {
		for(i = 0; i < FC_COLS; ++i) {
			DisposPtr(deal[i]);
		}
		
		DisposPtr(deal);
	}
}

/* Legal move requirements:
	1: If to is a LastCard, then val(to) == NULL OR
		val(to) - 1 == val(from) and to, from have alternating colours.
	2: If to is a storage, then val(to) == CARD_NULL
	3: If to is a foundation, then val(to) + 1 == val(from) OR 
		val(to) == NULL and val(from) == 1 and have the same suit.
*/
Boolean FreecellLegalMove(FCState *state, FCZone from, FCZone to) {
	Card cardFrom = GetCardAt(state, from);
	Card cardTo = GetCardAt(state, to);
	
	if(CARD_EMPTY(cardFrom)) 
		return false;
	
	switch(to.zone) {
	case FCZ_LASTCARD:
		return CARD_EMPTY(cardTo)
			|| (CARD_GETNUM(cardTo) - 1 == CARD_GETNUM(cardFrom)
				&& AlternatingColours(cardTo, cardFrom));
				
	case FCZ_COLUMN:
		/* Can refer to empty columns as either */
		return CARD_EMPTY(cardTo) && CARD_GETNUM(cardFrom) == 13;
	
	case FCZ_STORAGE:
		return CARD_EMPTY(cardTo);
		
	case FCZ_FOUNDATION:
		return (CARD_EMPTY(cardTo) && CARD_GETNUM(cardFrom) == 1)
			|| (CARD_GETNUM(cardTo) + 1 == CARD_GETNUM(cardFrom)
				&& CARD_GETSUIT(cardTo) == CARD_GETSUIT(cardFrom));
	}
	
	return false;
}

Boolean FreecellPlayMove(FCState *state, FCZone from, FCZone to) {
	if(!FreecellLegalMove(state, from, to))
		return false;
	
	FreecellForceMove(state, from, to);
	++state->moves;
	state->lastMove[0] = from;
	state->lastMove[1] = to;
	return true;
}

void FreecellForceMove(FCState *state, FCZone from, FCZone to) {
	Card moveCard;
	
	switch(from.zone) {
	case FCZ_LASTCARD:
		moveCard = RemoveColumnCard(state->cols[from.num]);
		break;
	
	case FCZ_STORAGE:
		moveCard = state->store[from.num];
		state->store[from.num] = CARD_NULL;
		break;
		
	case FCZ_FOUNDATION:
		moveCard = state->foundation[from.num];
		if(CARD_GETNUM(moveCard) == 1) {
			state->foundation[from.num] = CARD_NULL;
		} else {
			ushort num = CARD_GETNUM(moveCard) - 1;
			CARD_SETNUM(state->foundation[from.num], num);
		}
		break;
	}
	
	switch(to.zone) {
	case FCZ_LASTCARD:
		AddColumnCard(&state->cols[to.num], moveCard);
		break;
	
	case FCZ_STORAGE:
		state->store[to.num] = moveCard;
		break;
		
	case FCZ_FOUNDATION:
		state->foundation[to.num] = moveCard;
		break;
	}
}

void FreecellUndoMove(FCState *state) {
	FCZone tmp;
	
	if(state->lastMove[0].zone == FCZ_NONE)
		return;
		
	FreecellForceMove(state, state->lastMove[1], state->lastMove[0]);
	--state->moves;
	tmp = state->lastMove[0];
	state->lastMove[0] = state->lastMove[1];
	state->lastMove[1] = tmp;
}	

void SequenceDeck(Card *deck) {
	ushort i;
	deck[0] = TO_CARD(0, 3, 13);
	for(i = 1; i < CARD_QTY; ++i) {
		if(CARD_GETSUIT(deck[i-1]) == 0)
			deck[i] = TO_CARD(0, 3, CARD_GETNUM(deck[i-1]) - 1);
		else
			deck[i] = deck[i-1] - TO_CARD(0, 1, 0);
	}
}

void AddColumnCard(FCColumn **col, Card card) {
	FCColumn *workCol = *col;
	Size ptrSize, reqSize;
	
	if(workCol->qty >= FC_DCSIZE) {
		ptrSize = GetPtrSize(workCol);
		reqSize = SIZEOF_FCCOLUMN(workCol->qty+1);
		if(reqSize > ptrSize) {
			*col = ExpandColumnSize(workCol, ptrSize);
			workCol = *col;
		}
	}
	
	workCol->cards[workCol->qty] = card;
	++workCol->qty;
}

static Card GetColumnCard(FCColumn *col) {
	if(col->qty == 0)
		return CARD_NULL;
		
	return col->cards[col->qty-1];
}

Card RemoveColumnCard(FCColumn *col) {
	Card card;
	
	if(col->qty == 0)
		return CARD_NULL;
	
	--col->qty;
	card = col->cards[col->qty];
	col->cards[col->qty] = CARD_NULL;
	return card;
}

FCColumn *ExpandColumnSize(FCColumn *old, Size ptrSize) {
	FCColumn *new = (FCColumn *) NewPtr(ptrSize + FC_CEXPAND * sizeof(Card));
	memcpy(new, old, ptrSize);
	return new;
}

Card GetCardAt(FCState *state, FCZone elem) {
	switch(elem.zone) {
	case FCZ_LASTCARD:
	case FCZ_COLUMN:
		return GetColumnCard(state->cols[elem.num]);
	
	case FCZ_STORAGE:
		return state->store[elem.num];
		
	case FCZ_FOUNDATION:
		return state->foundation[elem.num];
	}
	
	return CARD_NULL;
}

Boolean AlternatingColours(Card a, Card b) {
	if(CARD_EMPTY(a) || CARD_EMPTY(b))
		return false;
	
	if(CARD_GETSUIT(a) == C_DIAMOND || CARD_GETSUIT(a) == C_HEART) {
		return CARD_GETSUIT(b) == C_CLUB || CARD_GETSUIT(b) == C_SPADE;
	}
		
	return CARD_GETSUIT(b) == C_DIAMOND || CARD_GETSUIT(b) == C_HEART;
}
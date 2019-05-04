#ifndef GAMETYPES_H
#define GAMETYPES_H

/* Card Type: ffssnnnn. High order can be used for flags. */
#define CARD_FMASK 0xC0
#define CARD_SMASK 0x30
#define CARD_NMASK 0x0F

#define CARD_GETFLAGS(c) ((Byte) ((c & CARD_FMASK) >> 6))
#define CARD_GETSUIT(c) ((Suit) ((c & CARD_SMASK) >> 4))
#define CARD_GETNUM(c) ((short) (c & CARD_NMASK))
#define CARD_SETFLAGS(c, f) ((c) = ((c & ~CARD_FMASK) | ((f) << 6)))
#define CARD_SETSUIT(c, s) ((c) = ((c & ~CARD_SMASK) | ((s) << 4)))
#define CARD_SETNUM(c, n) ((c) = ((c & ~CARD_NMASK) | (n)))
#define TO_CARD(f, s, n) ((Card) (((f) << 6) | ((s) << 4) | (n)))

#define CARD_NULL ((Card) 0)
#define CARD_EMPTY(c) (CARD_GETNUM(c) == 0)
#define CARD_QTY 52

typedef Byte Card;

typedef enum Suit {
	C_CLUB=0,
	C_DIAMOND,
	C_HEART,
	C_SPADE
} Suit;


#endif /* GAMETYPES_H */
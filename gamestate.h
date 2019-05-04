#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "freecell.h"

struct GlobalState {
	FCState fcGame;
	Boolean running;
};

extern struct GlobalState gstate;

#endif /* GAMESTATE_H */
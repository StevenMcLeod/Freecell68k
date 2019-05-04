/* TODO
	- Bug where starting a new game causes some cards to not be drawn properly
		and may leave residual cards in foundation / storage
	- Optimize DlogOpenGame string to int
*/

#include "gamewind.h"
#include "gameintf.h"
#include "gamemenu.h"
#include "gamestate.h"
#include "freecell.h"

struct GlobalState gstate = {0};
static Rect dragRect;

static void InitMacintosh(void);
static void InitGameState(void);
static void HandleEvent(short eventMask);
static void HandleMouseDown(EventRecord *theEvent);
static void HandleContentClick(Point mousePt);
short RndRange(short lower, short upper);

void InitMacintosh(void) {
	MaxApplZone();
	
	InitGraf(&thePort);
	InitFonts();
	InitWindows();
	InitMenus();
	TEInit();
	InitDialogs(0L);
	randSeed = RndSeed;
	
	FlushEvents(everyEvent, 0);
	InitCursor();
}

void InitGameState(void) {
	FreecellInit(&gstate.fcGame);
	gstate.running = true;
}

void HandleEvent(short eventMask) {
	int			res;
	EventRecord	theEvent;

	HiliteMenu(0);
	SystemTask();			/* Handle desk accessories */
	
	if (!GetNextEvent(eventMask, &theEvent)) return;
	switch (theEvent.what) {
	case mouseDown:
		HandleMouseDown(&theEvent);
		break;
		
	case keyDown:
		if(theEvent.modifiers & cmdKey) {
			MenuEvent(MenuKey(theEvent.message & 0xFF));
		}			
			
		/* FALLTHROUGH */
	case autoKey:
		break;
		
	case activateEvt:
		if(theEvent.modifiers & activeFlag) {
			MenuEditState(false);
		} else {
			MenuEditState(true);
		}
		break;
			
	case updateEvt:
		BeginUpdate((WindowPtr) theEvent.message);
		/*EraseRect(&((WindowPtr) theEvent.message)->portRect);*/
		DrawAll(&gstate.fcGame);
		EndUpdate((WindowPtr) theEvent.message);
		break;
	}
}

void HandleMouseDown(EventRecord *theEvent) {
	WindowPtr	theWindow;
	short		windowCode = FindWindow(theEvent->where, &theWindow);
	
    switch(windowCode) {
	case inSysWindow: 
		SystemClick (theEvent, theWindow);
		break;
	    
	case inDrag:
	  	DragWindow(theWindow, theEvent->where, &dragRect);
		break;
		
	case inMenuBar:
		MenuEvent(MenuSelect(theEvent->where));
		break;
	  	  
	case inContent:
  		if(theWindow != FrontWindow()) {
  			SelectWindow(theWindow);
  		} else {
  			GlobalToLocal(&theEvent->where);
  			switch(HandleGameClick(&gstate.fcGame, theEvent->where)) {
  			/*case FCCE_BADDRAG:*/
  			case FCCE_BADMOVE:
  				SysBeep(1);
  				break;
  				
  			default:
  				;
  			}
  		}
		break;
	  	
	case inGoAway:
		if(TrackGoAway(theWindow, theEvent->where)) {
			HideWindow(theWindow);
			gstate.running = false;
		}
		break;
	}
}

int main(void) {
	Point cardPoint = {10, 10};
	WindowPtr testWind;
	
	InitMacintosh();
	InitGameState();
	MenuCreate();
	
	SetRect(&dragRect, 4, 24, screenBits.bounds.right-4, 
		screenBits.bounds.bottom-4);
	
	testWind = WindCreateGame(&gstate.fcGame);
	GameNewGame(&gstate.fcGame, GAME_RANDOMSEED);
	
	while(gstate.running) {
		HandleEvent(everyEvent);
	}
	
	FreecellDisposeDeal(gstate.fcGame.cols);
}
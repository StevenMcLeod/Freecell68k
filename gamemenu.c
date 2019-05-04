#include "gamemenu.h"
#include "common.h"
#include "gamewind.h"
#include "gamestate.h"
#include "gameintf.h"

#define MBAR_ID 128

enum {
	appleID=128,
	fileID,
	editID
};

enum {
	apple_aboutID=1
};

enum {
	file_newID=1,
	file_openID,
	file_restartID,
	file_quitID=5
};

enum {
	edit_undoID=1,
	edit_cutID=3,
	edit_copyID,
	edit_pasteID,
	edit_clearID
};

static void DoAppleMenu(short item);
static void DoFileMenu(short item);
static void DoEditMenu(short item);

void MenuCreate(void) {
	Handle mHandle;
	mHandle = GetNewMBar(MBAR_ID);
	SetMenuBar(mHandle);
	DrawMenuBar();
	
	mHandle = (Handle) GetMHandle(appleID);
	AddResMenu((MenuHandle) mHandle, 'DRVR');
}

void MenuEvent(long menuitem) {
	short menuID = HIWORD(menuitem);
	short itemID = LOWORD(menuitem);
	
	switch(menuID) {
	case appleID:	DoAppleMenu(itemID); break;
	case fileID:	DoFileMenu(itemID); break;
	case editID:	DoEditMenu(itemID); break;
	}
}

void MenuEditState(Boolean active) {
	MenuHandle theMenu = GetMHandle(editID);
	if(active) {
		EnableItem(theMenu, edit_cutID);
		EnableItem(theMenu, edit_copyID);
		EnableItem(theMenu, edit_pasteID);
		EnableItem(theMenu, edit_clearID);
	} else {
		DisableItem(theMenu, edit_cutID);
		DisableItem(theMenu, edit_copyID);
		DisableItem(theMenu, edit_pasteID);
		DisableItem(theMenu, edit_clearID);
	}
}

void MenuUndoState(Boolean active) {
	MenuHandle theMenu = GetMHandle(editID);
	if(active) {
		EnableItem(theMenu, edit_undoID);
	} else {
		DisableItem(theMenu, edit_undoID);
	}
}

void DoAppleMenu(short item) {
	GrafPtr *oldPort;
	MenuHandle theMenu = GetMHandle(appleID);
	
	if(item > apple_aboutID) {
		StringPtr name = (StringPtr) NewPtr(sizeof(Str255));
		GetPort(&oldPort);
		GetItem(theMenu, item, name);
		OpenDeskAcc(name);
		SetPort(oldPort);
		DisposPtr(name);
	} else {
		DlogAbout();
	}
}

void DoFileMenu(short item) {
	MenuHandle theMenu = GetMHandle(fileID);
	switch(item) {
	case file_newID:
		GameNewGame(&gstate.fcGame, GAME_RANDOMSEED);
		break;
		
	case file_openID: {
		ushort openSeed = DlogOpenGame();
		if(openSeed != -1) {
			GameNewGame(&gstate.fcGame, openSeed);
		}
	}	break;
		
	case file_restartID:
		GameNewGame(&gstate.fcGame, GAME_RANDOMSEED);
		break;
		
	case file_quitID:
		gstate.running = false;
		break;
	}
}

void DoEditMenu(short item) {
	switch(item) {
	case edit_undoID:
		FreecellUndoMove(&gstate.fcGame);
		GameDrawMove(&gstate.fcGame, gstate.fcGame.lastMove[0],
				gstate.fcGame.lastMove[1]);
		break;
	}
}
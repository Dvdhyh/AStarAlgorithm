#include "Engine.h"
#pragma once

#define CM_MENU_FILE_NEW 2002
#define CM_MENU_FILE_START 2003
#define CM_MENU_FILE_FIND 2004
#define CM_MENU_FILE_DYNA 2005
#define CM_MENU_FILE_EXIT 2006
#define TIMERID 2007

class Globals {
public:
	HMENU hMenu;
	HMENU hSubMenu;
	HINSTANCE hInst;
	TheEngine theEngine;
};
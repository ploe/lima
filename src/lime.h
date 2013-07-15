#ifndef LIME_H
#define LIME_H

#include <stdio.h>
#include <stdlib.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

/*	Boolean types	*/
enum {
	YES = -1,
	NO
};

enum {
	TRUE = -1,
	FALSE
};

extern lua_State *L;

#include "Crew.h"
#include "Stage.h"
#include "Sprite.h"
#include "Player.h"

#endif

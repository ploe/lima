#ifndef LIME_H
#define LIME_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*	strmatch - because strcmp returning 0 is always so fucking confusing	*/
#define strmatch(a, b) !(strcmp(a, b))

/*	raw_getfield - gets a field from a table	*/
#define raw_getfield(x, f) lua_pushstring(L, f); lua_rawget(L, x)

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
#include "Actor.h"

#endif

#ifndef LIME_H
#define LIME_H

#include <stdio.h>
#include <stdlib.h>
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

#include "Cast.h"

#endif

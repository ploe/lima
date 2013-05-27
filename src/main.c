#include "lime.h"

/*	The Stage is the output of the program, kinda, it encapsulates the libs.
	It's in charge of the Lua state, SDL and other stuff the game needs to be a game.
	
	It should be the last thing on the stack to die, as when it does it frees Lua. */

lua_State *L;

Status STAGE(Castmember *stage) {
	Status stage_free(Castmember *stage) {
		lua_close(L);
	}
	stage->free = stage_free;

	Status stage_update(Castmember *stage) {
		return LIVE;
	}
//	stage->update = stage_update;

	L = luaL_newstate();
	luaL_openlibs(L);
}

Status WRAPER(Castmember *c) {
	static int i = 3;
	c->update = WRAPER;
	if(i--) {
		return LIVE;
	}
	return WRAP;
}

/*	Castmember function	*/

static int lnew_Castmember(lua_State *l) {
	if(lua_istable(L, -1)) {
		Castmember *c = new_Castmember(NULL);
		luaL_unref(L, LUA_REGISTRYINDEX, c->t);
		c->t = luaL_ref(L, LUA_REGISTRYINDEX);
		lua_rawgeti(L, LUA_REGISTRYINDEX, c->t);
	}
	else if(lua_isnil(L, -1)) lua_newtable(L);
	else fputs("Lua Castmember type has to be a table or nil", stderr);
}


int main(int argc, char *argv[]) {
	new_Castmember(STAGE);
	lua_pushcfunction(L, lnew_Castmember);
	lua_setglobal(L, "Castmember");
	luaL_loadfile(L, "./res/debug.lua");
	lua_call(L, 0, 0);
	while(perform()) continue;
	return 0;
}

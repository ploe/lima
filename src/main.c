#include "lime.h"

static int lnew_Crew(lua_State *L) {
	Crew *c;
	if(!(c = new_Crew(NULL))) {
		lua_pushnil(L);
		return 0;
		
	}
	if(lua_istable(L, -1)) {
		luaL_unref(L, LUA_REGISTRYINDEX, c->t);
		c->t = luaL_ref(L, LUA_REGISTRYINDEX);
	}
	else if(!lua_isnil(L, -1)) fputs("Lua Crew type has to be a table or nil", stderr);

	lua_rawgeti(L, LUA_REGISTRYINDEX, c->t);
	return 1;
}

/*	An Actor is a visible and tangible object	*/


/*static jmp_buf env;

static int lpanic(lua_State *L) {
	longjmp(env, -1);
	return 0;
}
*/

int main(int argc, char *argv[]) {
	new_Crew(STAGE);
	new_Crew(ACTORS);

	lua_register(L, "Crew", lnew_Crew);

	if(luaL_loadfile(L, "res/debug.lua") || lua_pcall(L, 0, 0, 0)) {
		fprintf(stderr, "Failed to load configuration file, so scrapped: %s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
	}

	while(perform()) continue;
	return 0;
}

#include "lime.h"

/*	The 'filehandles' for the Signal system are EMIT and PERSIST. The
	difference between the two is that signals that are set with emit are
	killed at the end of the frame. Whereas those set with persist are 
	persistent, which means once I get around to file I/O they'll save.

	In C and Lua they are set with the 'emit' and 'persist'	*/

static int EMIT = 0, PERSIST = 0;

static int get_Signal(int t, char *s) {
	lua_rawgeti(L, LUA_REGISTRYINDEX, t);
	lua_pushstring(L, s);
	lua_rawget(L, -2);
	lua_remove(L, -2);
	return !lua_isnil(L, -1);
}

/*	signal checks each signal table to see if we have an entry, we make 
	sure we only push one signal to the stack, which makes this function
	a wee bit queer lookin'	

	We check the EMIT table first to keep things fast for silly frame 
	based things... */

int signal(char *s) {
	if(get_Signal(EMIT, s)) return YES;
	lua_pop(L, 1);
	if(get_Signal(PERSIST, s)) return YES;
	return NO;
}

static int lsignal(lua_State *L) {
	#define msg 1
	if(lua_isstring(L, -1) && signal((char *) lua_tostring(L, msg)) ) return 1;
	return 0;
	#undef msg
}

/*	set_Signal requires the value to be pushed to the Lua stack. It absorbs
	the value for reasons.	*/

static void set_Signal(int t, const char *s) {
	lua_rawgeti(L, LUA_REGISTRYINDEX, t);
	lua_pushstring(L, s);
	lua_pushvalue(L, -3);
	lua_rawset(L, -3);
	lua_pop(L, 2);
}

void emit(char *s) {
	lua_pushboolean(L, TRUE);
	set_Signal(EMIT, s);
}

void persist(char *s) {
	lua_pushboolean(L, TRUE);
	set_Signal(PERSIST, s);
}

static int lset_Signal(int t) {	
	#define msg 1
	if(lua_istable(L, msg)) {
		lua_pushstring(L, "tag");
		lua_rawget(L, msg);
		if(lua_isstring(L, -1)) lua_pushvalue(L, msg);
		else lua_pushnil(L);
	}
	else if(lua_isstring(L, msg)) lua_pushboolean(L, TRUE);

	if(!lua_isnil(L, -1)) {
		set_Signal(t, lua_tostring(L, -2));
		return YES;
	}
	return NO;
	#undef msg
}

static int lemit(lua_State *L) {
	if(!lset_Signal(EMIT)) fputs("Can only 'emit' signals that are tables with a 'tag' field or a string.\n", stderr);
	return 0;
}

static int lpersist(lua_State *L) {
	if(!lset_Signal(PERSIST)) fputs("Can only 'persist' signals that are tables with a 'tag' field or a string.\n", stderr);
	return 0;
}

int main(int argc, char *argv[]) {
	new_Crew(STAGE);

	lua_newtable(L);
	EMIT = luaL_ref(L, LUA_REGISTRYINDEX);
	lua_register(L, "emit", lemit);

	lua_newtable(L);
	PERSIST = luaL_ref(L, LUA_REGISTRYINDEX);
	lua_register(L, "persist", lpersist);
	
	lua_register(L, "signal", lsignal);

	persist("arse");
	signal("arse");

	while(perform()) continue;
	return 0;
}

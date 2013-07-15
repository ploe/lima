#include "lime.h"
#include <setjmp.h>

/*	The Stage is the output of the program, kinda, it encapsulates the libs.
	It's in charge of the Lua state, SDL and other stuff the game needs to be a game.
	
	It should be the last thing on the stack to die, as when it does it frees Lua. */



static void stackdump() {
	int i;
	int top = lua_gettop(L);
	for(i = 1; i <= top; i++) {
		int t = lua_type(L, i);
		switch(t) {
			case LUA_TSTRING:
				printf("Lua string \"%s\"\n", lua_tostring(L, t));
			break;

			case LUA_TBOOLEAN:
				printf("Lua string \"%s\"\n", lua_toboolean(L, t) ? "true" : "false");
			break;

			case LUA_TNUMBER:
                printf("Lua string \"%g\"\n", lua_tonumber(L, t));
            break;

			default:
				printf("%s", lua_typename(L, t));
			break;

		}
	}
	printf("\n");
}

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
//	stackdump();
}

typedef struct Actor {
	Sprite *s;
	int t;
	struct Actor *next;

} Actor;

static Actor *top;

Actor *new_Actor(char *name) {
	Actor *a = malloc(sizeof(Actor));
	if(a) {
		memset(a, 0, sizeof(Actor));
		a->next = top;
		top = a;
		lua_newtable(L);
		a->t = luaL_ref(L, LUA_REGISTRYINDEX);
	}
	else fputs("Failed to allocate new Actor", stderr);
	return a;
}

static int lnew_Actor(lua_State *L) {
	Actor *c;
	if(!(c = new_Actor(NULL))) {
		lua_pushnil(L);
		return 0;
		
	}
	if(lua_istable(L, -1)) {
		luaL_unref(L, LUA_REGISTRYINDEX, c->t);
		c->t = luaL_ref(L, LUA_REGISTRYINDEX);
	}
	else if(!lua_isnil(L, -1)) fputs("Lua Actor type has to be a table or nil", stderr);

	lua_rawgeti(L, LUA_REGISTRYINDEX, c->t);
	return 1;
//	stackdump();
}

Status ACTORS(Crew *actors) {
	actors->update = ACTORS;
	Actor *a;
	int stack = lua_gettop(L);
	for(a = top; a != NULL; a = a->next) {
		lua_rawgeti(L, LUA_REGISTRYINDEX, a->t);
		if(lua_istable(L, -1)) {
			lua_getfield(L, -1, "animate");
			if(lua_isfunction(L, -1)) {
				lua_pushvalue(L, -2);
				lua_call(L, 1, -1);
				if(lua_isfunction(L, -1)) lua_setfield(L, -2, "animate");
				else if(!lua_isnil(L, -1)) fputs("An Actor's animate function must return another function, or nil. ", stderr);
			}
		}
		lua_settop(L, stack);
	}
}

/*static jmp_buf env;

static int lpanic(lua_State *L) {
	longjmp(env, -1);
	return 0;
}
*/

int main(int argc, char *argv[]) {
	new_Crew(STAGE);

	lua_register(L, "Crew", lnew_Crew);
	lua_register(L, "Actor", lnew_Actor);

	if(luaL_loadfile(L, "res/debug.lua") || lua_pcall(L, 0, 0, 0)) {
		fprintf(stderr, "Failed to load configuration file, so scrapped: %s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
	}

	while(perform()) continue;
	return 0;
}

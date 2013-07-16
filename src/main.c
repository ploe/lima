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

typedef struct Actor {
	Sprite *s;
	int t;
	struct Actor *next;

} Actor;

static Actor *top;

Actor *new_Actor(char *name) {
	Actor *a = calloc(1, sizeof(Actor));
	if(a) {
		a->next = top;
		top = a;
		lua_newtable(L);
		a->t = luaL_ref(L, LUA_REGISTRYINDEX);
	}
	else fputs("Failed to allocate new Actor", stderr);
	return a;
}

/*static void updateclip(Actor *a) {
	getfield_Actor(a, "w");
	getfield_Actor(a, "h");
	if(lua_isnumber(L, -1) && lua_isnumber(L, -2)) {
		a->s->clip.w = lua_tointeger(L, -2);
		a->s->clip.h = lua_tointeger(L, -1);
	}
	lua_pop(L, -2);
}*/

static int getfield_Actor(Actor *a, const char *key) {
	lua_rawgeti(L, LUA_REGISTRYINDEX, a->t);
	lua_getfield(L, -1, key);
	lua_remove(L, -2);
}

static int nilfield_Actor(Actor *a, const char *key) {
	lua_rawgeti(L, LUA_REGISTRYINDEX, a->t);
	lua_pushnil(L);
    lua_setfield(L, -2, key);
	lua_pop(L, 1);
}

/*	findstr_Actor - takes a key/value pair and cycles through the Actors
	looking for the first one whose table contains a match	*/

static Actor *findstr_Actor(const char *key, const char *value) {
	int t = lua_gettop(L);
	Actor *a;
	for(a = top; a != NULL; a = a->next) {
		getfield_Actor(a, key);
		if(lua_isstring(L, -1) && strmatch(lua_tostring(L, -1), value)) break;
	}
	lua_settop(L, t);
	return a;
}

static int lnew_Actor(lua_State *L) {
	Actor *a;
	if(!(a = new_Actor(NULL))) {
		lua_pushnil(L);
		return 0;
		
	}
	if(lua_istable(L, -1)) {
		luaL_unref(L, LUA_REGISTRYINDEX, a->t);
		a->t = luaL_ref(L, LUA_REGISTRYINDEX);

/*	we tear the Sprite information out of Lua. We do this because these
	attributes are only valid for init, the rest of the time they're stored
	in engine out of harm's way. */

		SDL_Rect clip = {0, 0, 0, 0};
		getfield_Actor(a, "w");
		getfield_Actor(a, "h");
		if(lua_isnumber(L, -1) && lua_isnumber(L, -2)) {
			clip.w = lua_tointeger(L, -2);
 			clip.h = lua_tointeger(L, -1);
		}
		nilfield_Actor(a, "w");
		nilfield_Actor(a, "h");
		lua_pop(L, -2);

		getfield_Actor(a, "costume");
		if(lua_isstring(L, -1)) a->s = new_Sprite(lua_tostring(L, -1), clip);
		nilfield_Actor(a, "costume");
		lua_pop(L, 1);
	}
	else if(!lua_isnil(L, -1)) fputs("Lua Actor type has to be a table or nil", stderr);

	lua_rawgeti(L, LUA_REGISTRYINDEX, a->t);
	return 1;
}

/*	lcostume will be a method for the Actor class in Lua, it's why its name
	is prepended with the little 'l' - anyway it'll be called like this:
	myke:costume("image.png")

	Since the clip is closer to the machine, I'll have to define a method for
	that too.	

	myke:clip(w, h)
myke = Actor {
	x = 300,
	y = 600
	w = 100,
	h = 100,
	tag = "Myke"
}
*/

/*	function Actor:costume(file)	*/

static int lcostume_Actor(lua_State *L) {
	if(lua_istable(L, -2)) {
		Actor *a = NULL;
		lua_getfield(L, -1, "tag");
		if(lua_isstring(L, -1) && lua_isstring(L, -2)) {
			a = findstr_Actor("tag", lua_tostring(L, -1));
			costume_Sprite(a->s, lua_tostring(L, -2));
			lua_pushboolean(L, TRUE);
			return 1;
		}
	}
	return 0;
}

Status ACTORS(Crew *actors) {
	actors->update = ACTORS;
	Actor *a;
	int t = lua_gettop(L);
	for(a = top; a != NULL; a = a->next) {
		static SDL_Rect offset = {0, 0, 0, 0};
		if(a->s) draw_Sprite(a->s, offset);
	}
	lua_settop(L, t);
	return LIVE;
}

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
	lua_register(L, "Actor", lnew_Actor);

	if(luaL_loadfile(L, "res/debug.lua") || lua_pcall(L, 0, 0, 0)) {
		fprintf(stderr, "Failed to load configuration file, so scrapped: %s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
	}

	while(perform()) continue;
	return 0;
}

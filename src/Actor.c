#include "lime.h"

static Actor *top = NULL;

static int lprototype_Actor;

Actor *new_Actor(char *name) {
	Actor *a = calloc(1, sizeof(Actor));
	if(a) {
		a->next = top;
		top = a;
		lua_newtable(L);
		lua_rawgeti(L, LUA_REGISTRYINDEX, lprototype_Actor);
		lua_setmetatable(L, -2);
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

/*	considering making these functions variadic, but go no call to at the
	moment.	*/

static int getfield_Actor(Actor *a, const char *key) {
	lua_rawgeti(L, LUA_REGISTRYINDEX, a->t);
	lua_getfield(L, -1, key);
	lua_remove(L, -2);
}

/*	sets Actor's field to top of stack	*/

static int setfield_Actor(Actor *a, const char *key) {
	lua_rawgeti(L, LUA_REGISTRYINDEX, a->t);
	lua_pushvalue(L, -2);
	lua_remove(L, -3);
	lua_setfield(L, -2, key);
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

		lua_pushnumber(L, 0);
		setfield_Actor(a, "reel");

		lua_pushnumber(L, 1);
		setfield_Actor(a, "frame");

		lua_pushnumber(L, 0);
		setfield_Actor(a, "ticks");
	
	}
	else if(!lua_isnil(L, -1)) fputs("Lua Actor type has to be a table or nil", stderr);

	lua_rawgeti(L, LUA_REGISTRYINDEX, a->t);
	lua_rawgeti(L, LUA_REGISTRYINDEX, lprototype_Actor);
	lua_setmetatable(L, -2);

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
	if(lua_istable(L, -1)) {
		Actor *a = NULL;
		lua_getfield(L, -1, "tag");
		if(lua_isstring(L, -1) && lua_isstring(L, -3)) {
			a = findstr_Actor("tag", lua_tostring(L, -1));
			costume_Sprite(a->s, lua_tostring(L, -3));
			lua_pushboolean(L, TRUE);
			return 1;
		}
	}
	return 0;
}

/* 	Actors table that wants working on needs to be at the top of the 
stack.	Otherwise we don't know what we're looking for */

static Actor *findActor() {
	Actor *a = NULL;
	if(lua_istable(L, -1)) {
		lua_getfield(L, -1, "tag");
		if(lua_isstring(L, -1)) a = findstr_Actor("tag", lua_tostring(L, -1));
	}
	return a;
}

/*	Increments the frame counter and resets the ticks counter so that we
	don't have to screw around with them in Lua.	*/

static void lnextframe(Actor *a) {
	getfield_Actor(a, "frame");
	if(lua_isnumber(L, -1)) {
		lua_pushnumber(L, lua_tointeger(L, -1) + 1);
		setfield_Actor(a, "frame");
	}
	lua_pushnumber(L, 0);
	setfield_Actor(a, "ticks");
}

/*	function Actor:nextclip()	*/

static lnextclip_Actor(lua_State *L) {
	Actor *a = findActor();
	if(a) {
		nextclip(a->s);
		lnextframe(a);
	}
	return 0;
}

/*	function Actor:nextclip()	*/

static lprevclip_Actor(lua_State *L) {
	Actor *a = findActor();
	if(a) {
		prevclip(a->s);
		lnextframe(a);
	}
	return 0;
}

static int ljumpreel_Actor(lua_State *L) {
	if(lua_istable(L, -1) && lua_isnumber(L, -2)) {
		int i = lua_tointeger(L, -2);
		lua_getfield(L, -1, "tag");
		if(lua_isstring(L, -1)) 
		{ 
			Actor *a = findstr_Actor("tag", lua_tostring(L, -1));
			jumpreel(a->s, i);
			lua_pushnumber(L, i);
			setfield_Actor(a, "reel");

/*	ticks, frame and reel in the Actor's table are dumb control variables.
	These values aren't necessarily required for the animate functions,
	they can be called anything. This just simplifies the implementation
	overall. */

			jumpclip(a->s, 0);
			lua_pushnumber(L, 1);
			setfield_Actor(a, "frame");

			lua_pushnumber(L, 0);
			setfield_Actor(a, "ticks");
		}
	}
	return 0;
}

Status update_actors(Crew *actors) {
	int t = lua_gettop(L);
	
	Actor *a;
	for(a = top; a != NULL; a = a->next) {

/*	Increments Actor's tick counter every frame as it removes it from the
	responsibilty from doing it in Lua - these values can be ignored
	though, or set.	*/

		getfield_Actor(a, "ticks");
		if(lua_isnumber(L, -1)) {
			lua_pushnumber(L, lua_tointeger(L, -1) + 1);
			setfield_Actor(a, "ticks");
		}
		lua_pop(L, 1);

/*	animate function returns animate functions, ad nauseum.	*/

		getfield_Actor(a, "animate");
		lua_rawgeti(L, LUA_REGISTRYINDEX, a->t);
		if(lua_isfunction(L, -2)) {
			lua_pcall(L, 1, 1, 0);
			setfield_Actor(a, "animate");
			lua_pop(L, 1);
		}
		else lua_pop(L, 2);

		getfield_Actor(a, "visible");
		if(lua_isboolean(L, -1) && lua_toboolean(L, -1)) { 
			getfield_Actor(a, "x");
			getfield_Actor(a, "y");
			int x = lua_isnumber(L, -2) ? lua_tointeger(L, -2) : 0;
			int y = lua_isnumber(L, -1) ? lua_tointeger(L, -1) : 0;
			SDL_Rect offset = {x, y, 0, 0};
			draw_Sprite(a->s, offset);
		}
	}
	lua_settop(L, t);
	return LIVE;
}

/*	sets method on table at top of stack	*/

static void setmethod(char *key, void *function) {
	if(lua_istable(L, -1)) {
		lua_pushcfunction(L, function);
		lua_setfield(L, -2, key);
	}
}

int lnewindex_Actor(lua_State *L) {
	const char *key = lua_tostring(L, -2);
	if(
		strmatch(key, "nextclip") ||
		strmatch(key, "prevclip") ||
		strmatch(key, "jumpreel") ||
		strmatch(key, "costume")
	) fprintf(stderr, "%s not set since it's an Actor method.\n", key);
	else lua_rawset(L, -3);
	return 0;
}

Status ACTORS(Crew *actors) {
	actors->update = update_actors;

/*	Actor constructor	*/
	lua_register(L, "Actor", lnew_Actor);

/*	metatable, __index and __newindex	*/
	lua_newtable(L);
	
	lua_newtable(L);
	setmethod("nextclip", lnextclip_Actor);
	setmethod("prevclip", lprevclip_Actor);
	setmethod("jumpreel", ljumpreel_Actor);
	setmethod("costume", lcostume_Actor);
	lua_setfield(L, -2, "__index");

	lua_pushcfunction(L, lnewindex_Actor);
	lua_setfield(L, -2, "__newindex");

	lprototype_Actor = luaL_ref(L, LUA_REGISTRYINDEX);

	return LIVE;
}

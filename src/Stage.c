#include "lima.h"

/*	The Stage is the output of the program, kinda, it encapsulates the libs.
	It's in charge of the Lua state, SDL and other stuff the game needs to be a game.
	
	It should be the last thing on the stack to die, as when it does it frees Lua. */

static int start = 0;

/*	screen is the window, port is the buffer, is it four times bigger than 
the screen to allow big things drawing on */

SDL_Surface *screen = NULL, *port = NULL;

#define LIME_STAGEWIDTH 640
#define LIME_STAGEHEIGHT 480
#define LIME_STAGEBPP 16
#define LIME_FPS 25
SDL_Rect LIME_SCREENDIM = {0, 0, LIME_STAGEWIDTH, LIME_STAGEHEIGHT};

/*	I think the viewport may need encapsulating
	viewport->surface = surface
	viewport->clip = region of the viewport to show on screen

	Why? Just tidies it up a little bit... Didn't think I'd need to but I
	completely totally do.

	Rather than create leeway for a buffer I should set MIN and MAX cap for
	the camera. Where the viewport will pull in to and if we overstep that 
	boundary we don't follow the player or whatever after that.

	What it means is that each scene can have the appropriate amount of 
	depth on either side of the stage to jump in to. Ahhh... complexity.
*/

SDL_Rect scene_dim = {0, 0, 0, 0};
int new_Scene(int w, int h) {
	if( (w >= LIME_STAGEWIDTH) && (h >= LIME_STAGEHEIGHT) ) {
		scene_dim.w = w; scene_dim.h = h;
		if(port) SDL_FreeSurface(port);
		port = SDL_CreateRGBSurface(SDL_HWSURFACE, w, h, LIME_STAGEBPP, 0, 0, 0, 0);
		if (port) return YES;
	}
	else fprintf(stderr, "Scene must be the size of the Stage, at least. (%d,%d)\n", LIME_STAGEWIDTH, LIME_STAGEHEIGHT);
	return NO;
}

static SDL_Rect viewport, viewport_buffer = {0, 0, LIME_STAGEWIDTH, LIME_STAGEHEIGHT};
void setviewport_Stage(int x, int y) {
	if(x < 0) x = 0;
	else if(x > scene_dim.w - LIME_STAGEWIDTH) x = scene_dim.w - LIME_STAGEWIDTH;
	viewport_buffer.x = x;

	if(y < 0)  y = 0;
	else if(y > scene_dim.h - LIME_STAGEHEIGHT)  y = scene_dim.h - LIME_STAGEHEIGHT;
	viewport_buffer.y = y;
}

SDL_Rect getviewport_Stage() {
	return viewport;
}

lua_State *L;

static Status stage_update(Crew *stage) {
	if( (1000/LIME_FPS) > (SDL_GetTicks() - start))
		SDL_Delay(1000/LIME_FPS - (SDL_GetTicks() - start));

	start = SDL_GetTicks();
	SDL_BlitSurface(port, &viewport, screen, &LIME_SCREENDIM);
	SDL_Flip(screen);
	viewport = viewport_buffer;
	SDL_FillRect(screen, &LIME_SCREENDIM, SDL_MapRGBA(screen->format, 0, 0, 0, 0));
	SDL_FillRect(port, &scene_dim, SDL_MapRGBA(screen->format, 0, 128, 128, 0));
	return LIVE;
}

static Status stage_free(Crew *stage) {
	lua_close(L);
	SDL_FreeSurface(port);
	SDL_FreeSurface(screen);
	SDL_Quit();
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
}

static int lstackdump(lua_State *L) {
	stackdump();	
	return 0;
}

/*	serialization function constants
	I haven't used an enum because I can curb their scope with a macro	

	TABLE is the table we want to serialize 
	ARRAY is the buffer where we store the numbered indices so we don't 
	try to render them as key-value pairs 
	KEY is the index for the TABLE
	VALUE is the value for TABLE[KEY]	*/

#define TABLE 1
#define ARRAY 2
#define KEY 3
#define VALUE 4

static void serialize_Array() {
	/*	push ARRAY to the stack	*/
	lua_newtable(L);
	int i, max = lua_rawlen(L, TABLE);
	for(i = 1; i <= max; i++) {
		lua_rawgeti(L, TABLE, i);
		switch(lua_type(L, -1)) {
			case LUA_TNUMBER:
				printf("%f,", lua_tonumber(L, -1));
			break;

			case LUA_TBOOLEAN:
				printf("%s,", lua_toboolean(L, -1) ? "true": "false" ); 
			break;

			case LUA_TSTRING:
				printf("\"%s\",", lua_tostring(L, -1)); 
			break;

			case LUA_TTABLE:
				serialize(-1);
				printf(","); 
			break;
		}
		lua_rawseti(L, ARRAY, i);

		lua_pushnil(L);
		lua_rawseti(L, TABLE, i);
	}
}

static void repopulate_Array() {
	int i, max = lua_rawlen(L, ARRAY);
	for(i = 1; i <= max; i++) {
		lua_rawgeti(L, ARRAY, i);
		lua_rawseti(L, TABLE, i);	
	}
}

static void serialize_Hash() {
	lua_pushnil(L);
	while(lua_next(L, TABLE)) {
		switch(lua_type(L, VALUE)) {
			case LUA_TNUMBER:
				printf("%s=%f,", lua_tostring(L, KEY), lua_tonumber(L, VALUE)); 
			break;

			case LUA_TBOOLEAN:
				printf("%s=%s,", lua_tostring(L, KEY), lua_toboolean(L, VALUE) ? "true": "false" ); 
			break;

			case LUA_TSTRING:
				printf("%s=\"%s\",", lua_tostring(L, KEY), lua_tostring(L, VALUE)); 
			break;

			case LUA_TTABLE:
				printf("%s=", lua_tostring(L, KEY));
				serialize(VALUE);
				printf(",");
			break;
		}
		lua_pop(L, 1);
	}
}

static int lserialize(lua_State *L) {	
	if(!lua_istable(L, TABLE)) return 0;

	printf("{");
	serialize_Array();
	serialize_Hash();
	repopulate_Array();
	printf("}");

	return 0;	
}

void serialize(int i) {
	lua_getglobal(L, "serialize");
	lua_pushvalue(L, i);
	lua_call(L, 1, 0);
}

#undef TABLE
#undef KEY
#undef VALUE
#undef ARRAY 


Status STAGE(Crew *stage) {
	stage->free = stage_free;
	stage->update = stage_update;

	SDL_Init(SDL_INIT_EVERYTHING);
	screen = SDL_SetVideoMode(LIME_STAGEWIDTH, LIME_STAGEHEIGHT, LIME_STAGEBPP, SDL_HWSURFACE | SDL_DOUBLEBUF);
	viewport = viewport_buffer;
	new_Scene(1366, 1366);
	start = SDL_GetTicks();

	L = luaL_newstate();
	luaL_openlibs(L);

	new_Crew(MOUSE);
	new_Crew(SIGNALS);
	new_Crew(ACTORS);
	new_Crew(CURSOR);
	new_Crew(KEYBOARD);

	lua_register(L, "Crew", lnew_Crew);
	lua_register(L, "stackdump", lstackdump);
	lua_register(L, "serialize", lserialize);

	if(luaL_loadfile(L, "res/debug.lua") || lua_pcall(L, 0, 0, 0)) {
		fprintf(stderr, "Failed to load configuration file, so scrapped: %s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
	}

	return LIVE;
}

/*	stackdump is a debug function. It prints the stack it goes 
	'[index]: [type] [value]'	*/

void stackdump() {
	int i;
	int t = lua_gettop(L);
	for(i = 1; i <= t; i++) {
		int t = lua_type(L, i);
		switch(t) {
			case LUA_TSTRING:
				fprintf(stderr, "%d: string => \"%s\" \n", i, lua_tostring(L, i));
			break;

			case LUA_TBOOLEAN:
				fprintf(stderr, "%d: boolean => %s \n", i, lua_toboolean(L, i) ? "true" : "false");
			break;

			case LUA_TNUMBER:
                		fprintf(stderr, "%d: number => %g \n", i, lua_tonumber(L, i));
			break;

			default:
				fprintf(stderr, "%d: %s \n", i, lua_typename(L, t));
			break;
		}
	}
	lua_settop(L, t);
}

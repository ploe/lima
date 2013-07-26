#include "lime.h"

/*	The Stage is the output of the program, kinda, it encapsulates the libs.
	It's in charge of the Lua state, SDL and other stuff the game needs to be a game.
	
	It should be the last thing on the stack to die, as when it does it frees Lua. */

static int start = 0;
SDL_Surface *screen = NULL;

#define LIME_STAGEWIDTH 640
#define LIME_STAGEHEIGHT 480

#define LIME_FPS 25

static SDL_Rect viewport = {0, 0, LIME_STAGEWIDTH, LIME_STAGEHEIGHT};
static SDL_Surface *surface = NULL;

void viewport_Stage(int x, int y) {
	viewport.x = x;
	viewport.y = y;
}

lua_State *L;

static Status stage_update(Crew *stage) {
	SDL_Rect clip = {0, 0, LIME_STAGEWIDTH, LIME_STAGEHEIGHT};
	if( (1000/LIME_FPS) > (SDL_GetTicks() - start))
		SDL_Delay(1000/LIME_FPS - (SDL_GetTicks() - start));

	start = SDL_GetTicks();
	SDL_Flip(screen);
	SDL_FillRect(screen, &clip, SDL_MapRGBA(screen->format, 0, 128, 128, 0));
	return LIVE;
}

static Status stage_free(Crew *stage) {
	lua_close(L);
	SDL_FreeSurface(screen);
	SDL_Quit();
}

static int lstackdump(lua_State *L) {
	stackdump();	
	return 0;
}

Status STAGE(Crew *stage) {
	stage->free = stage_free;
	stage->update = stage_update;

	SDL_Init(SDL_INIT_EVERYTHING);
	screen = SDL_SetVideoMode(LIME_STAGEWIDTH, LIME_STAGEHEIGHT, 16, SDL_HWSURFACE | SDL_DOUBLEBUF);
	start = SDL_GetTicks();

	L = luaL_newstate();
	luaL_openlibs(L);

	new_Crew(MOUSE);
	new_Crew(CURSOR);

	lua_register(L, "stackdump", lstackdump);

	return LIVE;
}

void stackdump() {
	int i;
	int t = lua_gettop(L);
	for(i = 1; i <= t; i++) {
		int t = lua_type(L, i);
		switch(t) {
			case LUA_TSTRING:
				fprintf(stderr, "string: \"%s\" \n", lua_tostring(L, i));
			break;

			case LUA_TBOOLEAN:
				fprintf(stderr, "boolean: %s \n", lua_toboolean(L, i) ? "true" : "false");
			break;

			case LUA_TNUMBER:
                fprintf(stderr, "number: %g \n", lua_tonumber(L, i));
            break;

			case LUA_TNIL:
                fprintf(stderr, "nil \n");
			break;

			default:
				fprintf(stderr, "%s \n", lua_typename(L, i));
			break;

		}
	}
	lua_settop(L, t);
}


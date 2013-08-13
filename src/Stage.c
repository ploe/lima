#include "lime.h"

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

static SDL_Rect viewport = {0, 0, LIME_STAGEWIDTH, LIME_STAGEHEIGHT};
void setviewport_Stage(int x, int y) {
	if(x < 0) x = 0;
	else if(x > scene_dim.w - LIME_STAGEWIDTH) x = scene_dim.w - LIME_STAGEWIDTH;
	viewport.x = x;

	if(y < 0) y = 0;
	else if(y > scene_dim.y) x = scene_dim.y;
	viewport.y = y;
}

SDL_Rect getviewport_Stage() {
	SDL_Rect v = viewport;
	return viewport;
}

lua_State *L;

static Status stage_update(Crew *stage) {
	if( (1000/LIME_FPS) > (SDL_GetTicks() - start))
		SDL_Delay(1000/LIME_FPS - (SDL_GetTicks() - start));

	start = SDL_GetTicks();
	SDL_BlitSurface(port, &viewport, screen, &LIME_SCREENDIM);
	SDL_Flip(screen);
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

static int lstackdump(lua_State *L) {
	stackdump();	
	return 0;
}

Status ViewShifter(Crew *viewshifter) {
	viewshifter->update = ViewShifter;
	static int x = 0, y = 0, vx = 8, vy = 8;
	if(x < 0 || x > scene_dim.w) vx = -vx;
	//if(y < 0 || y > scene_dim.h) vy = -vy;
	x -= vx;
	//y -= vy;
	setviewport_Stage(x, viewport.y);
	return LIVE;
}

Status STAGE(Crew *stage) {
	stage->free = stage_free;
	stage->update = stage_update;

	SDL_Init(SDL_INIT_EVERYTHING);
	screen = SDL_SetVideoMode(LIME_STAGEWIDTH, LIME_STAGEHEIGHT, LIME_STAGEBPP, SDL_HWSURFACE | SDL_DOUBLEBUF);
	new_Scene(800, 600);
	start = SDL_GetTicks();

	L = luaL_newstate();
	luaL_openlibs(L);

	setviewport_Stage(100, 100);

	new_Crew(MOUSE);
	new_Crew(CURSOR);
	new_Crew(ViewShifter);

	lua_register(L, "stackdump", lstackdump);
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

			default:
				fprintf(stderr, "%s \n", lua_typename(L, i));
			break;
		}
	}
	lua_settop(L, t);
}


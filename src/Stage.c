#include "lime.h"

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
//	if(LIME_FPS/1000 > (SDL_GetTicks() - start)) SDL_Delay(LIME_FPS/1000 - (SDL_GetTicks() - start));
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


	return LIVE;
}

#include "lime.h"

typedef struct Mouse {
	int x, y;
	void *over, *wheelhandler;	/*	Actor or Prop once defined	*/
} Mouse;

static Mouse mouse;

// static Status mouse_update = 

SDL_Joystick *joystick;
#define X_AXIS 0
#define Y_AXIS 1

static void pushbutton_Mouse(int button) {
	switch(button) {
		case SDL_BUTTON_LEFT: lua_pushstring(L, "left"); break;
		case SDL_BUTTON_RIGHT: lua_pushstring(L, "right"); break;
		case SDL_BUTTON_WHEELUP: lua_pushstring(L, "up"); break;
		case SDL_BUTTON_WHEELDOWN: lua_pushstring(L, "down"); break;
	}

	if(lua_isstring(L, -1)) {
		lua_pushboolean(L, YES);
		lua_rawset(L, -3);
		serialize(-1);
	}
}

static void move_Mouse() {
	lua_pushstring(L, "x");
	lua_pushnumber(L, mouse.x);
	lua_rawset(L, -3);

	lua_pushstring(L, "y");
	lua_pushnumber(L, mouse.y);
	lua_rawset(L, -3);	
} 

static Status update_player(Crew *this) {
	lua_getglobal(L, "emit");
	lua_newtable(L);

	lua_pushstring(L, "tag");
	lua_pushstring(L, "MOUSE");
	lua_rawset(L, -3);

	move_Mouse();

	SDL_Event event;
	while(SDL_PollEvent(&event)) {
		switch(event.type) {
			case SDL_QUIT:
				return WRAP;
			break;

			case SDL_MOUSEBUTTONDOWN:   /*  all button handling done by routines    */
				pushbutton_Mouse(event.button.button);
			break;			
	
			case SDL_MOUSEMOTION:
				mouse.x = event.motion.x;
				mouse.y = event.motion.y;
			break;
	
		}
	}
	lua_call(L, 1, 0);
	return LIVE;
}

Status MOUSE(Crew *this) {
	this->update = update_player;
	if(SDL_NumJoysticks() >= 1) joystick = SDL_JoystickOpen(0);
	return LIVE;
}

Status KEYBOARD(Crew *keyboard) {
	keyboard->update = KEYBOARD;
	static Uint8 *key;
	key = SDL_GetKeyState(NULL);

	if(key[SDLK_LEFT]) emit("PLAYER_LEFT");
	if(key[SDLK_RIGHT]) emit("PLAYER_RIGHT");

	if(key[SDLK_ESCAPE]) return WRAP;
	return LIVE;
}

Status GAMEPAD(Crew *this) {

	return LIVE;
}


SDL_Rect offset_mouse() {
	SDL_Rect tmp = {mouse.x, mouse.y};
	return tmp;
}

#define CURSOR_DIM 16
#define CURSOR_LAG 3	

static Sprite *cursor_sprite; 

static Status update_cursor(Crew *cursor) {
	SDL_Rect viewport = getviewport_Stage();
	SDL_Rect offset = {viewport.x + mouse.x - CURSOR_DIM / 2, viewport.y + mouse.y - CURSOR_DIM / 2};
	static int i = CURSOR_LAG;
	if(!i--) {
		if(cursor_sprite->clip.x) prevclip(cursor_sprite);
		else nextclip(cursor_sprite);
		i = CURSOR_LAG;
	}
//	static int frame = 0;
//	if(!i--) {
//		switch(frame++) {
//			case 0: case 1:
//				nextclip(cursor_sprite);
//			break;
//			case 3:
//				frame = 0;
//			case 2:
//				prevclip(cursor_sprite);
//			break;
//		}
//		i = CURSOR_LAG;
//	}
	draw_Sprite(cursor_sprite, offset);
	return LIVE;
}

static Status free_cursor(Crew *cursor) {
	free_Sprite(cursor_sprite);
}

Status CURSOR(Crew *cursor) {
	SDL_ShowCursor(SDL_DISABLE);
	SDL_Rect clip = {0, 0, CURSOR_DIM, CURSOR_DIM};
	cursor_sprite = new_Sprite("./res/cursor.png", clip);
	cursor->update = update_cursor;
	cursor->free = free_cursor;
	return LIVE;
}

#undef CURSOR_DIM
#undef CURSOR_LAG

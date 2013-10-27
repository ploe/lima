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

static Status update_player(Crew *this) {
	SDL_Event event;
    while(SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_QUIT:
                return WRAP;
            break;

            case SDL_MOUSEBUTTONDOWN:   /*  all button handling done by routines    */
                if((event.button.button == SDL_BUTTON_LEFT)) puts("left");
                else if((event.button.button == SDL_BUTTON_RIGHT)) puts("right");
                else if((event.button.button == SDL_BUTTON_WHEELUP)) puts("wheelup");
                else if((event.button.button == SDL_BUTTON_WHEELDOWN)) puts("wheeldown");
            break;

            case SDL_MOUSEMOTION:
                mouse.x = event.motion.x;
                mouse.y = event.motion.y;
            break;

			case SDL_JOYAXISMOTION:
				if((event.jaxis.which == 0)) {
					if(event.jaxis.axis == X_AXIS) {
						printf("x axis value: %d\n", event.jaxis.value);
						if ( ( event.jaxis.value > -8000 ) && ( event.jaxis.value < 8000 ) ) break;
						if(event.jaxis.value < 0) emit("PLAYER_LEFT");
						else if(event.jaxis.value > 0) emit("PLAYER_RIGHT");
					}
				}
			break;
        }
    }
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

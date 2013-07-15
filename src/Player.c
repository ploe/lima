#include "lime.h"

typedef struct Mouse {
	int x, y;
	void *over, *wheelhandler;	/*	Actor or Prop once defined	*/
} Mouse;

static Mouse mouse;

// static Status mouse_update = 

Status MOUSE(Crew *this) {
	if(this->update != MOUSE) {
		this->update = MOUSE;
	}
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
		}
	}
	mouse.over = NULL;

	return LIVE;
}


SDL_Rect offset_mouse() {
	SDL_Rect tmp = {mouse.x, mouse.y};
	return tmp;
}

#define CURSOR_DIM 100
#define CURSOR_LAG 3	

static Sprite *cursor_sprite; 

static Status update_cursor(Crew *cursor) {
	SDL_Rect offset = {mouse.x - CURSOR_DIM / 2, mouse.y - CURSOR_DIM / 2};
	static int i = CURSOR_LAG;
	static int frame = 0;
	if(!i--) {
		switch(frame++) {
			case 0: case 1:
				nextclip(cursor_sprite);
			break;
			case 3:
				frame = 0;
			case 2:
				prevclip(cursor_sprite);
			break;
		}
		i = CURSOR_LAG;
	}
	draw_Sprite(cursor_sprite, offset);
	return LIVE;
}

static Status free_cursor(Crew *cursor) {
	free_Sprite(cursor_sprite);
}


Status CURSOR(Crew *cursor) {
	SDL_Rect clip = {0, 0, CURSOR_DIM, CURSOR_DIM};
	cursor_sprite = new_Sprite("daisy-flat.png", clip);
	jumpreel(cursor_sprite, 1);
	cursor->update = update_cursor;
	cursor->free = free_cursor;
	return LIVE;
}

#undef CURSOR_DIM
#undef CURSOR_LAG

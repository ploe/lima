#include "lime.h"

/*	I've gone with pushing all sprites to the heap for this one reason, and it's a doozy.
	I don't have to pass an extra 's' param in to new_Sprite.
	Myke you dirty beast. ;)	*/

Sprite *new_Sprite(char *file, SDL_Rect clip) {
	Sprite *s;
	if(s = malloc(sizeof(Sprite))) {
		memset(s, 0, sizeof(Sprite));
		costume_Sprite(s, file);
		s->clip = clip;
	}
	return s;
	
}

/*	swaps out one surface for another	*/

void costume_Sprite(Sprite *s, char *file) {
	SDL_Surface *tmp = IMG_Load(file);
	if(tmp) {
		if(s->costume) SDL_FreeSurface(s->costume);
		s->costume =  SDL_DisplayFormatAlpha(tmp);
		SDL_FreeSurface(tmp);
		
	}	
}

void free_Sprite(Sprite *s) {
	if (!s) return;
	if (s->costume) SDL_FreeSurface(s->costume);
	free(s);
}

void draw_Sprite(Sprite *s, SDL_Rect offset) {
	SDL_BlitSurface(s->costume, &(s->clip), screen, &offset);
}

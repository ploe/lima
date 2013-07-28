#ifndef LIME_SPRITE_H
#define LIME_SPRITE_H

typedef struct Sprite {
	SDL_Surface *costume;
	SDL_Rect clip;
	int frame;
} Sprite;

/*	Sprite prototypes	*/

#define jumpreel(s, i) s->clip.y = s->clip.h * i
#define jumpclip(s, i) s->clip.x = s->clip.w * i
#define nextclip(s) s->clip.x += s->clip.w
#define prevclip(s) s->clip.x -= s->clip.w
#define jumpclip(s, i) s->clip.x = s->clip.w * i

Sprite *new_Sprite(const char *file, SDL_Rect clip);
void free_Sprite(Sprite *s);
void costume_Sprite(Sprite *s, const char *file);
void draw_Sprite(Sprite *s, SDL_Rect offset);

#endif



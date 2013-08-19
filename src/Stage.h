#ifndef LIME_STAGE_H
#define LIME_STAGE_H

extern SDL_Surface *port;

Status STAGE(Crew *);
void setviewport_Stage(int x, int y);
SDL_Rect getviewport_Stage();
void stackdump();

#endif

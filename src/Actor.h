#ifndef LIME_ACTOR_H
#define LIME_ACTOR_H

typedef struct Actor {
	Sprite *s;
	int t;
	struct Actor *next;

} Actor;

Actor *new_Actor(char *name);
Status ACTORS(Crew *);

#endif

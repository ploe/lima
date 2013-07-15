#ifndef LIME_CAST_H
#define LIME_CAST_H

/*	The nodes in the Crew stack were called Castmembers, however I preferred the brevity of the word Crew.
	The word Crew is also distinct from the word cast which already means something in C (typecasting).
	As such I kinda sorta copped out and the plural noun to refer to an individual Crewman, is weird but
	ultimately I prefer the way Crews looks in code. Don't judge me too harsh. =)

	Crewmen are the individual segments of action during a frame.
	A Crewman dictates its own life span, by returning different status codes in its update function
	The status codes are:
	* LIVE means the Crewman will live onto the next frame.
	* CUT means the Crewman will be freed.
	* PAUSE causes the Crewman to not execute its update function.
	* WRAP kills lime entirely	*/

typedef int Status;
enum {
    LIVE,
    CUT,
    PAUSE,
    WRAP
};

/*	A Crewman has three Constructor functions attached to it. If none are set, they default to NULL.
	* type is the Crewman's initialiser, and is also used to identify the type of Crew it is
	* update is the function the Crewman calls each frame. It returns a Status code.
	* free is the Crewman's deconstructor	

	Another feature of a Crewman is their Lua table 't' - we stuff this hash with all the relevant scripty goodness	
	If t contains a function called 'update' we call that every frame 
	However if the Crewman has a Constructor called update set we'll always default to using that first (because it's faster)	*/

typedef struct Crew Crew;
typedef Status (*Constructor)(Crew *);


typedef struct Crew {
	Constructor type, update, free;
	int t;
	Status status;
	struct Crew *next;
} Crew;

/* 
	* perform - The heartbeat of the program. What I call the Infinite State Machine.
	It's the function that monitors what each Crewman returns, and frees 'em or whatever.

	* new_Crew - Pushes a new Crewman to the top of the stack	 */

int perform();
Crew *new_Crew(Constructor new);

#endif

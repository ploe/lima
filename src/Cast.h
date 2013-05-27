#ifndef LIME_CAST_H
#define LIME_CAST_H

/*	Castmember's are the individual segments of action during a frame.
	A Castmember dictates its own life span, by returning different status codes in its update function
	The status codes are:
	* LIVE means the Castmember will live onto the next frame.
	* CUT means the Castmember will be freed.
	* PAUSE causes the Castmember to not execute its update function.
	* WRAP kills lime entirely	*/

typedef int Status;
enum {
    LIVE,
    CUT,
    PAUSE,
    WRAP
};

/*	A Castmember has three Constructor functions attached to it. If none are set, they default to NULL.
	* type is the Castmember's initialiser, and is also used to identify the type of Castmember it is
	* update is the function the Castmember calls each frame. It returns a Status code.
	* free is the Castmember's deconstructor	

	Another feature of a Castmember is their Lua table 't' - we stuff this hash with all the relevant scripty goodness	
	If t contains a function called 'update' we call that every frame 
	However if the Castmember has a Constructor called update set we'll always default to using that first (because it's faster)	*/

typedef struct Castmember Castmember;
typedef Status (*Constructor)(Castmember *);


typedef struct Castmember {
	Constructor type, update, free;
	int t;
	Status status;
	struct Castmember *next;
} Castmember;

/* 
	* perform - The heartbeat of the program. What I call the Infinite State Machine.
	It's the function that monitors what each Castmember returns, and frees 'em or whatever.

	* new_Castmember - Pushes a new Castmember to the top of the stack	 */

int perform();
Castmember *new_Castmember(Constructor new);

#endif

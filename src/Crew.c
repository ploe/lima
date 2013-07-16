#include "lime.h"
static Crew *top = NULL;

Crew *new_Crew(Constructor new) {
	Crew *c = calloc(1, sizeof(Crew));
	if(c) {
		c->next = top;
		top = c;

		c->type = new;
		c->free = c->update = NULL;

		if(new) c->status = new(c);

		lua_newtable(L);
		c->t = luaL_ref(L, LUA_REGISTRYINDEX);	
	}
	else fputs("Failed to allocate new Crew", stderr);
	return c;
}

/*	free_Crew - this shouldn't need calling directly as ideally a Crewman should be CUT
	Squeezes the Crewman from the stack, and calls its free functions */

static void free_Crew(Crew *this) {
	Crew *prev = NULL;
	Crew *c;
	
	for(c = top; c != NULL; c = c->next) {
		if(c == this) {
			if(this->next) {
				if(prev) prev->next = this->next;	/* 	middle node	*/
				else top = this->next;				/*	top			*/
			}
			else
			{
				if(prev) prev->next = NULL;			/*	bottom		*/
				else top = NULL;					/*	last		*/
			}
			luaL_unref(L, LUA_REGISTRYINDEX, this->t);
			if(this->free) this->free(this);
			free(this);
			return;
		}
		prev = c;
	}
}

static void purge() {
	while(top) {
		free_Crew(top);
	}
}

/*	lua_update - a Crewman isn't freed unless we say so, so lua_update returns LIVE by default
	update in the table just returns a string and we translate it to the correct status code	*/

static int getvalue_Crew(Crew *c, const char *key) {
	lua_rawgeti(L, LUA_REGISTRYINDEX, c->t);
	lua_getfield(L, -1, key);
}


/*	Since it's only called in one place its name is outside the usual convention of doit_Crew	*/
static Status lua_update(Crew *c) {
	int i = lua_gettop(L);
	getvalue_Crew(c, "update");

	if(lua_isfunction(L, -1)) {
		lua_pushvalue(L, -2);
		lua_remove(L, -3);
		lua_call(L, 1, -2);
	}

	if(lua_isstring(L, -1)) {
		const char *tmp = lua_tostring(L, -1);
		if(strmatch("LIVE", tmp)) return LIVE;
		else if(strmatch("CUT", tmp)) return CUT;
		else if(strmatch("PAUSE", tmp)) return PAUSE;
		else if(strmatch("WRAP", tmp)) return WRAP;
		else fprintf(stderr, "%s is not a valid Status. Treating it as LIVE.\n", tmp);
	}
	else fputs("Invalid 'update' type. Needs to be a function or a string.\n", stderr);

	lua_settop(L, i);
	return LIVE;
}

#undef strmatch

/*	I was thinking about making the Lua update function and the C one the same thing, by just registering the C
	function in the Lua State. However I'd need to pass in the details of the struct, and don't fancy cluttering
	up the C global namespace with it. 

	Since C updates are more for engine/framework housekeeping it doesn't make enough sense to make them hackable. 
	It would be a needless luxury, that'd require me to expose the guts to Lua. Which kind of defeats the point... 
	Hence why we look for a C function and then a Lua function.	*/

int perform() {
    Crew *c = top;
    while(c != NULL) {
		/*	if there isn't a C update function we dig one out of the Lua table */
        if(c->status == LIVE) c->status = c->update ? c->update(c) : lua_update(c);

        if(c->status == CUT) {
            Crew *next = c->next;
            free_Crew(c);
            if(!next) break;
            c = next;
        }
        else if(c->status == WRAP) {
            purge();
            return NO;
        }
        else c = c->next;
    }
    return YES;
}

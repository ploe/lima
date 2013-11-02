#include "lime.h"

/*
	serialize(t)
	foreach key/value pair
	print "[key]="
	switch type(value):
		case NUMBER or BOOLEAN: print value
		case STRING: print "value"
		case TABLE: print serialize(value)
*/

int main(int argc, char *argv[]) {
	new_Crew(STAGE);

	while(perform()) continue;
	return 0;
}

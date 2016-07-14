#pragma once

#define numberof(x)	(sizeof(x)/sizeof((x)[0]))
#define endof(x) ((x) + numberof(x))
#define frand() ((double)rand() / (double)RAND_MAX)
#define crand()	(frand() * 2. - 1.)


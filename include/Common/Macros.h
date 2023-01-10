#pragma once

#define numberof(x)	(sizeof(x)/sizeof((x)[0]))
#define endof(x) ((x) + numberof(x))
#define frand() ((double)rand() / (double)RAND_MAX)
#define crand()	(frand() * 2. - 1.)

//http://stackoverflow.com/questions/3859944/combining-string-literals-and-integer-constants
#define STRINGIZE_DETAIL_(v)	#v
#define STRINGIZE(v) STRINGIZE_DETAIL_(v)
#define LINE_STRING	STRINGIZE(__LINE__)
#define FILE_AND_LINE __FILE__ ":" LINE_STRING

//I need a unique api macro for *each project*
//otherwise how can I determine which class apis are import and which are export ...
//here's how a single library does it:
/*
#if defined(BUILDING_DLL)
#if defined(BUILDING_OUR_DLL)
#define API __declspec(dllexport)
#else
#define API __declspec(dllimport)
#endif
#else
#define API		extern
#endif
*/
//if a library is being built
// then define this macro as dllexport
//  and only give it to classes of this library
// (which means somehow specify other libraries' classes differently -- as import)

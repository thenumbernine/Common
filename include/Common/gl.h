/*
notice that Common doesn't depend on GL
I'm just putting this in one place for all projects to reference
I would've put it in GLApp, but then projects that use GL but not GLApp would still need to reference GLApp
*/
#pragma once

#ifdef PLATFORM_osx

//for GLhandleARB, etc
#include <OpenGL/OpenGL.h>
#include <OpenGL/glext.h>

#include <OpenGL/gl.h>

#elif PLATFORM_linux

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>

#endif

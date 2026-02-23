#ifndef _DEBUG
#define NDEBUG
#endif

#if defined(WIN32)  || defined(_WIN32)
#include <Windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#if USE_GLUT
#define FREEGLUT_STATIC
#include <GL/freeglut.h>
#endif
#elif defined(linux) || defined(__linux__)
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#elif defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#endif


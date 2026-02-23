/*

  USC/Viterbi/Computer Science
  "Jello Cube" Assignment 1 starter code

*/

#ifndef _INPUT_H_
#define _INPUT_H_
#include "types.h"

// camera parameters
extern double g_ftheta;
extern double g_fphi;
extern double g_fradius;

// mouse & keyboard control
void mouseMotionDrag(int x, int y);
void mouseMotion(int x, int y);

#if USE_GLUT
void mouseButton(int button, int state, int x, int y);
void keyboardFunc(unsigned char key, int x, int y);
#else
void mouseButton(int button, int state, int x, int y);
void keyboardFunc(GLFWwindow* window, int key, int scancode, int action, int mods);
#endif // #if USE_GLUT

// read/write world files
void readWorld(char* fileName, struct world* jello);
void writeWorld(char* fileName, struct world* jello);

#endif

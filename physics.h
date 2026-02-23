/*

  USC/Viterbi/Computer Science
  "Jello Cube" Assignment 1 starter code

*/

#ifndef _PHYSICS_H_
#define _PHYSICS_H_

#include "types.h"
void computeAcceleration(struct world* jello,
                         struct point a[JELLO_SUBPOINTS][JELLO_SUBPOINTS][JELLO_SUBPOINTS]);

// perform one step of Euler and Runge-Kutta-4th-order integrators
// updates the jello structure accordingly
void Euler(struct world* jello);
void RK4(struct world* jello);

#endif

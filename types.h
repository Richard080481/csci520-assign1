#ifndef _TYPES_H_
#define _TYPES_H_

#define JELLO_SUBPOINTS 8
#define JELLO_SUBDIVISIONS (JELLO_SUBPOINTS - 1)

// camera angles
extern double Theta;
extern double Phi;
extern double R;

// number of images saved to disk so far
extern int sprite;

// mouse control
extern int g_vMousePos[2];
extern int g_iLeftMouseButton, g_iMiddleMouseButton, g_iRightMouseButton;

struct point
{
    double x;
    double y;
    double z;
};

// these variables control what is displayed on the screen
extern int shear, bend, structural, pause, viewingMode, saveScreenToFile;

struct world
{
    char integrator[10]; // "RK4" or "Euler"
    double dt;           // timestep, e.g.. 0.001
    int n;               // display only every nth timepoint
    double kElastic;     // Hook's elasticity coefficient for all springs except collision springs
    double dElastic;     // Damping coefficient for all springs except collision springs
    double kCollision;   // Hook's elasticity coefficient for collision springs
    double dCollision;   // Damping coefficient collision springs
    double mass; // mass of each of the (JELLO_SUBPOINTS^3) control points, mass assumed to be equal
                 // for every control point
    int incPlanePresent; // Is the inclined plane present? 1 = YES, 0 = NO (always NO in this
                         // assignment)
    double a, b, c, d; // inclined plane has equation a * x + b * y + c * z + d = 0; if no inclined
                       // plane, these four fields are not used
    int resolution;    // resolution for the 3d grid specifying the external force field; value of 0
                       // means that there is no force field
    struct point* forceField; // pointer to the array of values of the force field
    struct point p[JELLO_SUBPOINTS][JELLO_SUBPOINTS]
                  [JELLO_SUBPOINTS]; // position of the JELLO_SUBPOINTS^3 control points
    struct point v[JELLO_SUBPOINTS][JELLO_SUBPOINTS]
                  [JELLO_SUBPOINTS]; // velocities of the JELLO_SUBPOINTS^3 control points
};

extern struct world jello;
#endif
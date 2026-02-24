#ifndef _TYPES_H_
#define _TYPES_H_

#define NEW_VULKAN 1

// Use glut on 32-bit Windows, and use GLFW on 64-bit Windows and all other platforms. This is because the 64-bit version of glut is old and does not support modern OpenGL features, which causes compilation errors on 64-bit Windows. On 32-bit Windows, we
// can use the newer version of glut without issues. However, for Vulkan build, we should use GLFW.
#if _M_X64
#define VULKAN_BUILD 1
#else // #if _M_X64
#define VULKAN_BUILD 0
#endif // #if _M_X64

#if VULKAN_BUILD
#define USE_GLUT 0
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define NOMINMAX
#include "Windows.h"
#else // #if VULKAN_BUILD
#define USE_GLUT 1
#include "openGL-headers.h"
#endif // #if VULKAN_BUILD

#define JELLO_SUBPOINTS 8
#define JELLO_SUBDIVISIONS (JELLO_SUBPOINTS - 1)

#define PI 3.141592653589793238462643383279

struct point
{
    double x;
    double y;
    double z;
};

// these variables control what is displayed on the screen
extern int g_ishear;
extern int g_ibend;
extern int g_istructural;
extern int g_ipause;
extern int g_iviewingMode;
extern int g_isaveScreenToFile;

extern int g_istep; // render number of frames and stop
extern int g_iphysics; // do physics

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

#endif
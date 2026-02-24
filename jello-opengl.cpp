/*

  USC/Viterbi/Computer Science
  "Jello Cube" Assignment 1 starter code

  Your name:
  Richard Wang

*/

#include "jello-opengl.h"

#include <assert.h>
#include <math.h>

#include "input.h"
#include "jelloApp.h"
#include "physics.h"
#include "pic.h"
#include "showCube.h"

static int g_iwindowWidth, g_iwindowHeight;

#if !VULKAN_BUILD

struct world g_jello;

void myinit()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(90.0, 1.0, 0.01, 1000.0);

    // set background color to grey
    glClearColor(0.5, 0.5, 0.5, 0.0);

    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    glShadeModel(GL_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glEnable(GL_LINE_SMOOTH);

    return;
}

void reshape(int w, int h)
{
    // Prevent a divide by zero, when h is zero.
    // You can't make a window of zero height.
    if (h == 0)
        h = 1;

    glViewport(0, 0, w, h);

    // Reset the coordinate system before modifying
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Set the perspective
    double aspectRatio = 1.0 * w / h;
    gluPerspective(60.0f, aspectRatio, 0.01f, 1000.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    g_iwindowWidth = w;
    g_iwindowHeight = h;

#if USE_GLUT
    glutPostRedisplay();
#else
#endif
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // camera parameters are Phi, Theta, R
    gluLookAt(g_fradius * cos(g_fphi) * cos(g_ftheta), g_fradius * sin(g_fphi) * cos(g_ftheta), g_fradius * sin(g_ftheta), 0.0, 0.0, 0.0,
              0.0, 0.0, 1.0);

    /* Lighting */
    /* You are encouraged to change lighting parameters or make improvements/modifications
       to the lighting model .
       This way, you will personalize your assignment and your assignment will stick out.
    */

    // global ambient light
    GLfloat aGa[] = {0.0, 0.0, 0.0, 0.0};

    // light 's ambient, diffuse, specular
    GLfloat lKa0[] = {0.0, 0.0, 0.0, 1.0};
    GLfloat lKd0[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat lKs0[] = {1.0, 1.0, 1.0, 1.0};

    GLfloat lKa1[] = {0.0, 0.0, 0.0, 1.0};
    GLfloat lKd1[] = {1.0, 0.0, 0.0, 1.0};
    GLfloat lKs1[] = {1.0, 0.0, 0.0, 1.0};

    GLfloat lKa2[] = {0.0, 0.0, 0.0, 1.0};
    GLfloat lKd2[] = {1.0, 1.0, 0.0, 1.0};
    GLfloat lKs2[] = {1.0, 1.0, 0.0, 1.0};

    GLfloat lKa3[] = {0.0, 0.0, 0.0, 1.0};
    GLfloat lKd3[] = {0.0, 1.0, 1.0, 1.0};
    GLfloat lKs3[] = {0.0, 1.0, 1.0, 1.0};

    GLfloat lKa4[] = {0.0, 0.0, 0.0, 1.0};
    GLfloat lKd4[] = {0.0, 0.0, 1.0, 1.0};
    GLfloat lKs4[] = {0.0, 0.0, 1.0, 1.0};

    GLfloat lKa5[] = {0.0, 0.0, 0.0, 1.0};
    GLfloat lKd5[] = {1.0, 0.0, 1.0, 1.0};
    GLfloat lKs5[] = {1.0, 0.0, 1.0, 1.0};

    GLfloat lKa6[] = {0.0, 0.0, 0.0, 1.0};
    GLfloat lKd6[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat lKs6[] = {1.0, 1.0, 1.0, 1.0};

    GLfloat lKa7[] = {0.0, 0.0, 0.0, 1.0};
    GLfloat lKd7[] = {0.0, 1.0, 1.0, 1.0};
    GLfloat lKs7[] = {0.0, 1.0, 1.0, 1.0};

    // light positions and directions
    GLfloat lP0[] = {-1.999, -1.999, -1.999, 1.0};
    GLfloat lP1[] = {1.999, -1.999, -1.999, 1.0};
    GLfloat lP2[] = {1.999, 1.999, -1.999, 1.0};
    GLfloat lP3[] = {-1.999, 1.999, -1.999, 1.0};
    GLfloat lP4[] = {-1.999, -1.999, 1.999, 1.0};
    GLfloat lP5[] = {1.999, -1.999, 1.999, 1.0};
    GLfloat lP6[] = {1.999, 1.999, 1.999, 1.0};
    GLfloat lP7[] = {-1.999, 1.999, 1.999, 1.0};

    // jelly material color

    GLfloat mKa[] = {0.0, 0.0, 0.0, 1.0};
    GLfloat mKd[] = {0.3, 0.3, 0.3, 1.0};
    GLfloat mKs[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat mKe[] = {0.0, 0.0, 0.0, 1.0};

    /* set up lighting */
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, aGa);
    glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
    glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);

    // set up cube color
    glMaterialfv(GL_FRONT, GL_AMBIENT, mKa);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mKd);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mKs);
    glMaterialfv(GL_FRONT, GL_EMISSION, mKe);
    glMaterialf(GL_FRONT, GL_SHININESS, 120);

// macro to set up light i
#define LIGHTSETUP(i)                                                                              \
    glLightfv(GL_LIGHT##i, GL_POSITION, lP##i);                                                    \
    glLightfv(GL_LIGHT##i, GL_AMBIENT, lKa##i);                                                    \
    glLightfv(GL_LIGHT##i, GL_DIFFUSE, lKd##i);                                                    \
    glLightfv(GL_LIGHT##i, GL_SPECULAR, lKs##i);                                                   \
    glEnable(GL_LIGHT##i)

    LIGHTSETUP(0);
    LIGHTSETUP(1);
    LIGHTSETUP(2);
    LIGHTSETUP(3);
    LIGHTSETUP(4);
    LIGHTSETUP(5);
    LIGHTSETUP(6);
    LIGHTSETUP(7);

    // enable lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);

    // show the cube
    showCube(&g_jello);

    glDisable(GL_LIGHTING);

    // show the bounding box
    showBoundingBox();

#if USE_GLUT
    glutSwapBuffers();
#else
#endif
}

/* Write a screenshot to the specified filename, in PPM format */
void saveScreenshot(int windowWidth, int windowHeight, char* filename)
{
    if (filename == NULL)
        return;

    // Allocate a picture buffer
    Pic* in = pic_alloc(windowWidth, windowHeight, 3, NULL);

    printf("File to save to: %s\n", filename);

    for (int i = windowHeight - 1; i >= 0; i--)
    {
        glReadPixels(0, windowHeight - i - 1, windowWidth, 1, GL_RGB, GL_UNSIGNED_BYTE, &in->pix[i * in->nx * in->bpp]);
    }

    if (ppm_write(filename, in))
        printf("File saved Successfully\n");
    else
        printf("Error in Saving\n");

    pic_free(in);
}

void doIdle()
{
    static int sprite = 0; // number of images saved to disk so far
    static double timeCounter = 0.0;

    char s[20] = "picxxxx.ppm";
    int i;

    // save screen to file
    s[3] = 48 + (sprite / 1000);
    s[4] = 48 + (sprite % 1000) / 100;
    s[5] = 48 + (sprite % 100) / 10;
    s[6] = 48 + sprite % 10;

    if (g_isaveScreenToFile == 1)
    {
        if (timeCounter >= (1.0 / 15))
        {
            saveScreenshot(g_iwindowWidth, g_iwindowHeight, s);
            timeCounter -= (1.0 / 15);
            sprite++;
        }
        // saveScreenToFile=0; // save only once, change this if you want continuos image generation
        // (i.e. animation)
        timeCounter += 0.0005;
    }

    if (sprite >= 300) // allow only 300 snapshots
    {
        exit(0);
    }

    if (g_ipause == 0)
    {
        // perform one time step of the simulation
        if (strcmp(g_jello.integrator, "Euler") == 0)
        {
            Euler(&g_jello);
        }
        else
        {
            RK4(&g_jello);
        }
    }

#if USE_GLUT
    glutPostRedisplay();
#else
#endif
}

#if USE_GLUT
int main(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("Oops! You didn't say the g_jello world file!\n");
        printf("Usage: %s [worldfile]\n", argv[0]);
        assert(0 && "Oops! You didn't say the g_jello world file!");
        exit(0);
    }

    readWorld(argv[1], &g_jello);

    g_iwindowWidth = 640;
    g_iwindowHeight = 480;

    glutInit(&argc, argv);

    /* double buffered window, use depth testing, 640x480 */
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowSize(g_iwindowWidth, g_iwindowHeight);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Jello cube");

    /* tells glut to use a particular display function to redraw */
    glutDisplayFunc(display);

    /* replace with any animate code */
    glutIdleFunc(doIdle);

    /* callback for mouse drags */
    glutMotionFunc(mouseMotionDrag);

    /* callback for window size changes */
    glutReshapeFunc(reshape);

    /* callback for mouse movement */
    glutPassiveMotionFunc(mouseMotion);

    /* callback for mouse button changes */
    glutMouseFunc(mouseButton);

    /* register for keyboard events */
    glutKeyboardFunc(keyboardFunc);

    myinit();

    /* forever sink in the black hole */
    glutMainLoop();

    return (0);
}
#endif // #if USE_GLUT
#endif // #if !VULKAN_BUILD
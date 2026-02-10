/*

  USC/Viterbi/Computer Science
  "Jello Cube" Assignment 1 starter code

*/

#include "physics.h"
#include "jello.h"

/* Computes acceleration to every control point of the jello cube,
   which is in state given by 'jello'.
   Returns result in array 'a'. */

// Helper function to compute spring force between two points
void computeSpringForce(point p1, point p2, point v1, point v2, double restLength, double kHook,
                        double kDamp, point* force)
{
    // Vector from p1 to p2
    point L;
    pDIFFERENCE(p1, p2, L);

    // Current length
    double length = sqrt(L.x * L.x + L.y * L.y + L.z * L.z);

    if (length < 1e-8)
    {
        return; // Avoid division by zero
    }

    // Unit vector
    point unitL;
    pMULTIPLY(L, 1.0 / length, unitL);

    // Spring force magnitude: F = -k * (length - restLength)
    point springForceVec;
    double springMagnitude = -kHook * (length - restLength);
    pMULTIPLY(unitL, springMagnitude, springForceVec);

    // Damping force: F = -kd * ((v1-v2) dot unitL)
    point vDiff;
    pDIFFERENCE(v1, v2, vDiff);
    double dotProduct = vDiff.x * unitL.x + vDiff.y * unitL.y + vDiff.z * unitL.z;
    double dampingMagnitude = -kDamp * dotProduct;

    point dampingForceVec;
    pMULTIPLY(unitL, dampingMagnitude, dampingForceVec);

    // Add both forces
    pSUM(*force, springForceVec, *force);
    pSUM(*force, dampingForceVec, *force);
}

void addStructuralForces(struct world* jello, int i, int j, int k, point* force)
{
    // Connect to 6 immediate neighbors (x, y, z directions)
    int neighbors[6][3] = {
        {1, 0, 0}, {-1, 0, 0}, // x direction
        {0, 1, 0}, {0, -1, 0}, // y direction
        {0, 0, 1}, {0, 0, -1}  // z direction
    };

    // Rest length is 1.0 (grid spacing between adjacent points)
    double restLength = 1.0 / JELLO_SUBDIVISIONS;

    for (int n = 0; n < 6; n++)
    {
        int ni = i + neighbors[n][0];
        int nj = j + neighbors[n][1];
        int nk = k + neighbors[n][2];

        // Check bounds
        if (ni >= 0 && ni <= JELLO_SUBDIVISIONS && nj >= 0 && nj <= JELLO_SUBDIVISIONS && nk >= 0 &&
            nk <= JELLO_SUBDIVISIONS)
        {
            computeSpringForce(jello->p[i][j][k], jello->p[ni][nj][nk], jello->v[i][j][k],
                               jello->v[ni][nj][nk], restLength, jello->kElastic, jello->dElastic,
                               force);
        }
    }
}

void addShearForces(struct world* jello, int i, int j, int k, point* force)
{
    // Face diagonals (12 springs per point)
    int planeShearNeighbors[12][3] = {// xy plane diagonals
                                      {1, 1, 0},
                                      {1, -1, 0},
                                      {-1, 1, 0},
                                      {-1, -1, 0},
                                      // xz plane diagonals
                                      {1, 0, 1},
                                      {1, 0, -1},
                                      {-1, 0, 1},
                                      {-1, 0, -1},
                                      // yz plane diagonals
                                      {0, 1, 1},
                                      {0, 1, -1},
                                      {0, -1, 1},
                                      {0, -1, -1}};

    // Rest length for face diagonals: sqrt(2)
    double planeRestLength = sqrt(2.0) / JELLO_SUBDIVISIONS;

    for (int n = 0; n < 12; n++)
    {
        int ni = i + planeShearNeighbors[n][0];
        int nj = j + planeShearNeighbors[n][1];
        int nk = k + planeShearNeighbors[n][2];

        if (ni >= 0 && ni <= JELLO_SUBDIVISIONS && nj >= 0 && nj <= JELLO_SUBDIVISIONS && nk >= 0 &&
            nk <= JELLO_SUBDIVISIONS)
        {
            computeSpringForce(jello->p[i][j][k], jello->p[ni][nj][nk], jello->v[i][j][k],
                               jello->v[ni][nj][nk], planeRestLength, jello->kElastic,
                               jello->dElastic, force);
        }
    }

    int bodyShearNeighbors[8][3] = {{1, 1, 1},  {1, 1, -1},  {1, -1, 1},  {1, -1, -1},
                                    {-1, 1, 1}, {-1, 1, -1}, {-1, -1, 1}, {-1, -1, -1}};

    double bodyRestLength = sqrt(3.0) / JELLO_SUBDIVISIONS;

    for (int n = 0; n < 8; n++)
    {
        int ni = i + bodyShearNeighbors[n][0];
        int nj = j + bodyShearNeighbors[n][1];
        int nk = k + bodyShearNeighbors[n][2];

        if (ni >= 0 && ni <= JELLO_SUBDIVISIONS && nj >= 0 && nj <= JELLO_SUBDIVISIONS && nk >= 0 &&
            nk <= JELLO_SUBDIVISIONS)
        {
            computeSpringForce(jello->p[i][j][k], jello->p[ni][nj][nk], jello->v[i][j][k],
                               jello->v[ni][nj][nk], bodyRestLength, jello->kElastic,
                               jello->dElastic, force);
        }
    }
}

void addBendForces(struct world* jello, int i, int j, int k, point* force)
{
    // Connect to points 2 units away in each axis direction
    int bendNeighbors[6][3] = {{2, 0, 0}, {-2, 0, 0}, {0, 2, 0}, {0, -2, 0}, {0, 0, 2}, {0, 0, -2}};

    // Rest length for bend springs: 2.0
    double restLength = 2.0 / JELLO_SUBDIVISIONS;

    for (int n = 0; n < 6; n++)
    {
        int ni = i + bendNeighbors[n][0];
        int nj = j + bendNeighbors[n][1];
        int nk = k + bendNeighbors[n][2];

        if (ni >= 0 && ni <= JELLO_SUBDIVISIONS && nj >= 0 && nj <= JELLO_SUBDIVISIONS && nk >= 0 &&
            nk <= JELLO_SUBDIVISIONS)
        {
            computeSpringForce(jello->p[i][j][k], jello->p[ni][nj][nk], jello->v[i][j][k],
                               jello->v[ni][nj][nk], restLength, jello->kElastic, jello->dElastic,
                               force);
        }
    }
}

void addForceFieldForce(struct world* jello, int i, int j, int k, point* force)
{
    if (jello->forceField == NULL)
    {
        return;
    }

    point p = jello->p[i][j][k];

    // Map position to grid coordinates [0, resolution-1]
    // Assuming the jello cube is positioned in [0, JELLO_SUBDIVISIONS] range (natural grid
    // coordinates)
    double x_grid = p.x;
    double y_grid = p.y;
    double z_grid = p.z;

    // Clamp to valid range
    if (x_grid < 0)
    {
        x_grid = 0;
    }
    if (x_grid > jello->resolution - 1)
    {
        x_grid = jello->resolution - 1;
    }
    if (y_grid < 0)
    {
        y_grid = 0;
    }
    if (y_grid > jello->resolution - 1)
    {
        y_grid = jello->resolution - 1;
    }
    if (z_grid < 0)
    {
        z_grid = 0;
    }
    if (z_grid > jello->resolution - 1)
    {
        z_grid = jello->resolution - 1;
    }

    // Trilinear interpolation
    int x0 = (int)floor(x_grid);
    int y0 = (int)floor(y_grid);
    int z0 = (int)floor(z_grid);
    int x1 = (x0 < jello->resolution - 1) ? x0 + 1 : x0;
    int y1 = (y0 < jello->resolution - 1) ? y0 + 1 : y0;
    int z1 = (z0 < jello->resolution - 1) ? z0 + 1 : z0;

    double xd = x_grid - x0;
    double yd = y_grid - y0;
    double zd = z_grid - z0;

    // Bounds checking for array access
    if (x0 < 0 || x1 >= jello->resolution || y0 < 0 || y1 >= jello->resolution || z0 < 0 ||
        z1 >= jello->resolution)
    {
        return;
    }

    // Get force field values at 8 corners
    int stride = jello->resolution * jello->resolution;
    point f000 = jello->forceField[x0 + y0 * jello->resolution + z0 * stride];
    point f100 = jello->forceField[x1 + y0 * jello->resolution + z0 * stride];
    point f010 = jello->forceField[x0 + y1 * jello->resolution + z0 * stride];
    point f110 = jello->forceField[x1 + y1 * jello->resolution + z0 * stride];
    point f001 = jello->forceField[x0 + y0 * jello->resolution + z1 * stride];
    point f101 = jello->forceField[x1 + y0 * jello->resolution + z1 * stride];
    point f011 = jello->forceField[x0 + y1 * jello->resolution + z1 * stride];
    point f111 = jello->forceField[x1 + y1 * jello->resolution + z1 * stride];

    // Trilinear interpolation using macros
    point f00, f01, f10, f11, f0, f1, f;

    // Interpolate in x direction
    f00.x = f000.x * (1 - xd) + f100.x * xd;
    f00.y = f000.y * (1 - xd) + f100.y * xd;
    f00.z = f000.z * (1 - xd) + f100.z * xd;

    f01.x = f001.x * (1 - xd) + f101.x * xd;
    f01.y = f001.y * (1 - xd) + f101.y * xd;
    f01.z = f001.z * (1 - xd) + f101.z * xd;

    f10.x = f010.x * (1 - xd) + f110.x * xd;
    f10.y = f010.y * (1 - xd) + f110.y * xd;
    f10.z = f010.z * (1 - xd) + f110.z * xd;

    f11.x = f011.x * (1 - xd) + f111.x * xd;
    f11.y = f011.y * (1 - xd) + f111.y * xd;
    f11.z = f011.z * (1 - xd) + f111.z * xd;

    // Interpolate in y direction
    f0.x = f00.x * (1 - yd) + f10.x * yd;
    f0.y = f00.y * (1 - yd) + f10.y * yd;
    f0.z = f00.z * (1 - yd) + f10.z * yd;

    f1.x = f01.x * (1 - yd) + f11.x * yd;
    f1.y = f01.y * (1 - yd) + f11.y * yd;
    f1.z = f01.z * (1 - yd) + f11.z * yd;

    // Interpolate in z direction
    f.x = f0.x * (1 - zd) + f1.x * zd;
    f.y = f0.y * (1 - zd) + f1.y * zd;
    f.z = f0.z * (1 - zd) + f1.z * zd;

    // Add interpolated force
    pSUM(*force, f, *force);
}

void addCollisionForces(struct world* jello, int i, int j, int k, point* force)
{
    point p = jello->p[i][j][k];
    point v = jello->v[i][j][k];

    // Bounding box: [-2, 2] range
    double xMin = -2.0, xMax = 2.0;
    double yMin = -2.0, yMax = 2.0;
    double zMin = -2.0, zMax = 2.0;

    double penetration;

    // X faces - wall at xMin (x = -2)
    if (p.x < xMin)
    {
        penetration = xMin - p.x;                    // Positive: how far past the wall
        force->x += jello->kCollision * penetration; // Push out (positive direction)
        if (v.x < 0)                                 // Also moving into wall
        {
            force->x += jello->dCollision * fabs(v.x); // Add damping force (positive)
        }
    }
    // X faces - wall at xMax (x = 2)
    if (p.x > xMax)
    {
        penetration = p.x - xMax;                    // Positive: how far past the wall
        force->x -= jello->kCollision * penetration; // Push back (negative direction)
        if (v.x > 0)                                 // Also moving into wall
        {
            force->x -= jello->dCollision * fabs(v.x); // Subtract damping force (negative)
        }
    }

    // Y faces - wall at yMin (y = -2)
    if (p.y < yMin)
    {
        penetration = yMin - p.y;
        force->y += jello->kCollision * penetration;
        if (v.y < 0)
        {
            force->y += jello->dCollision * fabs(v.y);
        }
    }
    // Y faces - wall at yMax (y = 2)
    if (p.y > yMax)
    {
        penetration = p.y - yMax;
        force->y -= jello->kCollision * penetration;
        if (v.y > 0)
        {
            force->y -= jello->dCollision * fabs(v.y);
        }
    }

    // Z faces - wall at zMin (z = -2)
    if (p.z < zMin)
    {
        penetration = zMin - p.z;
        force->z += jello->kCollision * penetration;
        if (v.z < 0)
        {
            force->z += jello->dCollision * fabs(v.z);
        }
    }
    // Z faces - wall at zMax (z = 2)
    if (p.z > zMax)
    {
        penetration = p.z - zMax;
        force->z -= jello->kCollision * penetration;
        if (v.z > 0)
        {
            force->z -= jello->dCollision * fabs(v.z);
        }
    }
}

void computeAcceleration(struct world* jello,
                         point a[JELLO_SUBPOINTS][JELLO_SUBPOINTS][JELLO_SUBPOINTS])
{
    int i, j, k;
    point force;

    // Initialize all accelerations
    for (i = 0; i <= JELLO_SUBDIVISIONS; i++)
    {
        for (j = 0; j <= JELLO_SUBDIVISIONS; j++)
        {
            for (k = 0; k <= JELLO_SUBDIVISIONS; k++)
            {
                pMAKE(0.0, 0.0, 0.0, a[i][j][k]);
            }
        }
    }

    // Compute forces for each mass point
    for (i = 0; i <= JELLO_SUBDIVISIONS; i++)
    {
        for (j = 0; j <= JELLO_SUBDIVISIONS; j++)
        {
            for (k = 0; k <= JELLO_SUBDIVISIONS; k++)
            {
                // Reset force accumulator
                pMAKE(0.0, 0.0, 0.0, force);

                addStructuralForces(jello, i, j, k, &force);

                addShearForces(jello, i, j, k, &force);

                addBendForces(jello, i, j, k, &force);

                if (jello->resolution != 0)
                {
                    addForceFieldForce(jello, i, j, k, &force);
                }

                addCollisionForces(jello, i, j, k, &force);

                pMULTIPLY(force, 1.0 / jello->mass, a[i][j][k]);
            }
        }
    }
}

/* performs one step of Euler Integration */
/* as a result, updates the jello structure */
void Euler(struct world* jello)
{
    int i, j, k;
    point a[JELLO_SUBPOINTS][JELLO_SUBPOINTS][JELLO_SUBPOINTS];

    computeAcceleration(jello, a);

    for (i = 0; i <= JELLO_SUBDIVISIONS; i++)
    {
        for (j = 0; j <= JELLO_SUBDIVISIONS; j++)
        {
            for (k = 0; k <= JELLO_SUBDIVISIONS; k++)
            {
                jello->p[i][j][k].x += jello->dt * jello->v[i][j][k].x;
                jello->p[i][j][k].y += jello->dt * jello->v[i][j][k].y;
                jello->p[i][j][k].z += jello->dt * jello->v[i][j][k].z;
                jello->v[i][j][k].x += jello->dt * a[i][j][k].x;
                jello->v[i][j][k].y += jello->dt * a[i][j][k].y;
                jello->v[i][j][k].z += jello->dt * a[i][j][k].z;
            }
        }
    }
}

/* performs one step of RK4 Integration */
/* as a result, updates the jello structure */
void RK4(struct world* jello)
{
    point F1p[JELLO_SUBPOINTS][JELLO_SUBPOINTS][JELLO_SUBPOINTS],
        F1v[JELLO_SUBPOINTS][JELLO_SUBPOINTS][JELLO_SUBPOINTS],
        F2p[JELLO_SUBPOINTS][JELLO_SUBPOINTS][JELLO_SUBPOINTS],
        F2v[JELLO_SUBPOINTS][JELLO_SUBPOINTS][JELLO_SUBPOINTS],
        F3p[JELLO_SUBPOINTS][JELLO_SUBPOINTS][JELLO_SUBPOINTS],
        F3v[JELLO_SUBPOINTS][JELLO_SUBPOINTS][JELLO_SUBPOINTS],
        F4p[JELLO_SUBPOINTS][JELLO_SUBPOINTS][JELLO_SUBPOINTS],
        F4v[JELLO_SUBPOINTS][JELLO_SUBPOINTS][JELLO_SUBPOINTS];

    point a[JELLO_SUBPOINTS][JELLO_SUBPOINTS][JELLO_SUBPOINTS];

    struct world buffer;

    int i, j, k;

    buffer = *jello; // make a copy of jello

    computeAcceleration(jello, a);

    for (i = 0; i <= JELLO_SUBDIVISIONS; i++)
    {
        for (j = 0; j <= JELLO_SUBDIVISIONS; j++)
        {
            for (k = 0; k <= JELLO_SUBDIVISIONS; k++)
            {
                pMULTIPLY(jello->v[i][j][k], jello->dt, F1p[i][j][k]);
                pMULTIPLY(a[i][j][k], jello->dt, F1v[i][j][k]);
                pMULTIPLY(F1p[i][j][k], 0.5, buffer.p[i][j][k]);
                pMULTIPLY(F1v[i][j][k], 0.5, buffer.v[i][j][k]);
                pSUM(jello->p[i][j][k], buffer.p[i][j][k], buffer.p[i][j][k]);
                pSUM(jello->v[i][j][k], buffer.v[i][j][k], buffer.v[i][j][k]);
            }
        }
    }

    computeAcceleration(&buffer, a);

    for (i = 0; i <= JELLO_SUBDIVISIONS; i++)
    {
        for (j = 0; j <= JELLO_SUBDIVISIONS; j++)
        {
            for (k = 0; k <= JELLO_SUBDIVISIONS; k++)
            {
                // F2p = dt * buffer.v;
                pMULTIPLY(buffer.v[i][j][k], jello->dt, F2p[i][j][k]);
                // F2v = dt * a(buffer.p,buffer.v);
                pMULTIPLY(a[i][j][k], jello->dt, F2v[i][j][k]);
                pMULTIPLY(F2p[i][j][k], 0.5, buffer.p[i][j][k]);
                pMULTIPLY(F2v[i][j][k], 0.5, buffer.v[i][j][k]);
                pSUM(jello->p[i][j][k], buffer.p[i][j][k], buffer.p[i][j][k]);
                pSUM(jello->v[i][j][k], buffer.v[i][j][k], buffer.v[i][j][k]);
            }
        }
    }

    computeAcceleration(&buffer, a);

    for (i = 0; i <= JELLO_SUBDIVISIONS; i++)
    {
        for (j = 0; j <= JELLO_SUBDIVISIONS; j++)
        {
            for (k = 0; k <= JELLO_SUBDIVISIONS; k++)
            {
                // F3p = dt * buffer.v;
                pMULTIPLY(buffer.v[i][j][k], jello->dt, F3p[i][j][k]);
                // F3v = dt * a(buffer.p,buffer.v);
                pMULTIPLY(a[i][j][k], jello->dt, F3v[i][j][k]);
                pMULTIPLY(F3p[i][j][k], 1.0, buffer.p[i][j][k]);
                pMULTIPLY(F3v[i][j][k], 1.0, buffer.v[i][j][k]);
                pSUM(jello->p[i][j][k], buffer.p[i][j][k], buffer.p[i][j][k]);
                pSUM(jello->v[i][j][k], buffer.v[i][j][k], buffer.v[i][j][k]);
            }
        }
    }
    computeAcceleration(&buffer, a);

    for (i = 0; i <= JELLO_SUBDIVISIONS; i++)
    {
        for (j = 0; j <= JELLO_SUBDIVISIONS; j++)
        {
            for (k = 0; k <= JELLO_SUBDIVISIONS; k++)
            {
                // F3p = dt * buffer.v;
                pMULTIPLY(buffer.v[i][j][k], jello->dt, F4p[i][j][k]);
                // F3v = dt * a(buffer.p,buffer.v);
                pMULTIPLY(a[i][j][k], jello->dt, F4v[i][j][k]);

                pMULTIPLY(F2p[i][j][k], 2, buffer.p[i][j][k]);
                pMULTIPLY(F3p[i][j][k], 2, buffer.v[i][j][k]);
                pSUM(buffer.p[i][j][k], buffer.v[i][j][k], buffer.p[i][j][k]);
                pSUM(buffer.p[i][j][k], F1p[i][j][k], buffer.p[i][j][k]);
                pSUM(buffer.p[i][j][k], F4p[i][j][k], buffer.p[i][j][k]);
                pMULTIPLY(buffer.p[i][j][k], 1.0 / 6, buffer.p[i][j][k]);
                pSUM(buffer.p[i][j][k], jello->p[i][j][k], jello->p[i][j][k]);

                pMULTIPLY(F2v[i][j][k], 2, buffer.p[i][j][k]);
                pMULTIPLY(F3v[i][j][k], 2, buffer.v[i][j][k]);
                pSUM(buffer.p[i][j][k], buffer.v[i][j][k], buffer.p[i][j][k]);
                pSUM(buffer.p[i][j][k], F1v[i][j][k], buffer.p[i][j][k]);
                pSUM(buffer.p[i][j][k], F4v[i][j][k], buffer.p[i][j][k]);
                pMULTIPLY(buffer.p[i][j][k], 1.0 / 6, buffer.p[i][j][k]);
                pSUM(buffer.p[i][j][k], jello->v[i][j][k], jello->v[i][j][k]);
            }
        }
    }

    return;
}

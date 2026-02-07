<Please submit this file with your solution.>

CSCI 520, Assignment 1

Richard Wang

================

Implemented features

- Implemented computeAcceleration (in physics.cpp): computes per-particle
    accelerations from structural, shear, and bend springs using Hooke's law,
    includes linear damping, applies external force-field interpolation, and
    handles collision response with the six bounding-box walls.
- Supported both Euler and RK4 integrators (physics.cpp) and verified they
    work with the acceleration implementation.

Extra credit
- Parameterized the particle grid: the simulation now supports an arbitrary
    N x N x N cube (configurable instead of the fixed 8x8x8 grid).
- Implemented automatic screenshot capture: `saveScreenShot` now records
    frames at 15 fps (one frame every 1/15 second).


computeAcceleration
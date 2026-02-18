<Please submit this file with your solution.>

CSCI 520, Assignment 1

Richard Wang

================

- Animation jpegs are in animation folder

Implemented features

- Implemented computeAcceleration (in physics.cpp): computes per-particle
    accelerations from structural, shear, and bend springs using Hooke's law,
    includes linear damping, applies external force-field interpolation, and
    handles collision response with the six bounding-box walls.
- Supported both Euler and RK4 integrators (physics.cpp) and verified they
    work with the computeAcceleration implementation.

Extra credit
- Parameterized the particle grid in OpenGL: the simulation now supports an arbitrary
    N x N x N cube (configurable instead of the fixed 8x8x8 grid).

- Upgrade from glut to glfw.

## Vulkan Graphics Pipeline

I have implemented a complete **Vulkan graphics pipeline**. To run the Vulkan version, please ensure you have the %Vulkan_SDK% installed on your system path.

Download Link: [Vulkan SDK](https://vulkan.lunarg.com)

## Visual Studio Configuration
To switch between rendering pipelines in Visual Studio:
x64: Enables Vulkan rendering.
x86: Runs the original OpenGL pipeline.

### Implementation Details
If you'd like to review the Vulkan source code, please refer to the following files:
* `vk_jello.h`
* `vk_jello.cpp`
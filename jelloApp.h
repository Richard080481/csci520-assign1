#ifndef _JELLO_APP_H_
#define _JELLO_APP_H_

#include "types.h"

#if VULKAN_BUILD

class Renderer;

class JelloApp
{
public:
    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;

    void run();
    void readWorld(char* fileName);
    void drawFrame();
    void setFramebufferResized(bool resized);

private:
    void createWindow();
    void createRenderer();
    void mainLoop();
    void destroyWindow();
    void destroyRenderer();

    GLFWwindow*     m_hwindow = nullptr;
    struct world    m_jello = {};
    Renderer*       m_pRenderer = nullptr;
    uint32_t        m_currentFrame = 0;
};

#endif // #if VULKAN_BUILD

#endif // #ifndef _JELLO_APP_H_

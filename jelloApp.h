#ifndef _JELLO_APP_H_
#define _JELLO_APP_H_

#include "types.h"
#include "input.h"
#include "renderer-vk.h"

#if VULKAN_BUILD

class Renderer;

class JelloScene
{
public:
    JelloScene(char* fileName)
    {
        ::readWorld(fileName, &m_jello);
    }

    const std::vector<Vertex>& getVertexData();
    const std::vector<uint16_t>& getIndexData();
    const IndexBufferInfo& getIndexBufferInfo();
    void initVerticesAndIndices();
    void doPhysics();

private:
    struct world            m_jello = {};
    IndexBufferInfo         m_jelloIndexBufferInfo = {};
    std::vector<uint16_t>   m_jelloIndices;
    std::vector<Vertex>     m_jelloVertices;
};

class JelloApp
{
public:
    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;

    JelloApp(char* fileName);
    ~JelloApp();

    void run();
    void createScene(char* fileName);
    void drawFrame();
    void setFramebufferResized(bool resized);

private:
    void createWindow();
    void createRenderer();
    void mainLoop();
    void destroyWindow();
    void destroyRenderer();

    GLFWwindow*     m_hwindow = nullptr;
    JelloScene*     m_pScene = nullptr;
    Renderer*       m_pRenderer = nullptr;
    uint32_t        m_currentFrame = 0;
};

#endif // #if VULKAN_BUILD

#endif // #ifndef _JELLO_APP_H_

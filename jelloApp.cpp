#include "jelloApp.h"

#if VULKAN_BUILD
#include <cassert>
#include <cstdio>

#include <exception>
#include <iostream>
#include <unordered_map>
#include <vector>

#include "input.h"
#include "physics.h"
#include "renderer-vk.h"
//#include "renderer-opengl.h" ///@todo Add OpenGL renderer.

const std::vector<Vertex>& JelloScene::getVertexData()
{
    ///@todo only update vertex data when necessary, e.g. when the jello cube is updated by the physics engine.
    ///      For now, we will update the vertex data every frame for simplicity.
    const glm::vec3 black = {0.0f, 0.0f, 0.0f};
    std::vector<Vertex> jelloVertices;
    int currentIndex = 0;

    for (int i = 0; i < JELLO_SUBPOINTS; i++)
    {
        for (int j = 0; j < JELLO_SUBPOINTS; j++)
        {
            for (int k = 0; k < JELLO_SUBPOINTS; k++)
            {
                if (i * j * k * (JELLO_SUBDIVISIONS - i) * (JELLO_SUBDIVISIONS - j) * (JELLO_SUBDIVISIONS - k) == 0)
                {
                    Vertex vertex = {{m_jello.p[i][j][k].x, m_jello.p[i][j][k].y, m_jello.p[i][j][k].z}, black};
                    jelloVertices.push_back(vertex);
                    currentIndex++;
                }
            }
        }
    }

    if (m_jelloVertices.size() == 0)
    {
        m_jelloVertices.resize(jelloVertices.size());
    }
    else
    {
        assert(jelloVertices.size() == m_jelloVertices.size());
    }

    memcpy(m_jelloVertices.data(), jelloVertices.data(), jelloVertices.size() * sizeof(jelloVertices[0]));

    return m_jelloVertices;
}

const std::vector<uint16_t>& JelloScene::getIndexData()
{
    return m_jelloIndices;
}

const IndexBufferInfo& JelloScene::getIndexBufferInfo()
{
    return m_jelloIndexBufferInfo;
}

void JelloScene::initVerticesAndIndices()
{
    const glm::vec3 black = {0.0f, 0.0f, 0.0f};
    std::vector<Vertex> jelloVertices;
    std::vector<uint16_t> jelloIndices[4];
    int currentIndex = 0;

    // isOnSurface(x, y, z) checks if the point at (x, y, z) is on the surface of the jello cube
    auto isOnSurface = [](int x, int y, int z) -> bool
    {
        return (x * y * z * (JELLO_SUBDIVISIONS - x) * (JELLO_SUBDIVISIONS - y) * (JELLO_SUBDIVISIONS - z) == 0);
    };

    // calIndex(i, j, k) is used to map the 3D indices (i, j, k) to a unique integer index for the LUT
    auto calIndex = [](int i, int j, int k) -> int
    {
        return i * JELLO_SUBPOINTS * JELLO_SUBPOINTS + j * JELLO_SUBPOINTS + k;
    };

    std::unordered_map<int, int> LUT;

    for (int i = 0; i < JELLO_SUBPOINTS; i++)
    {
        for (int j = 0; j < JELLO_SUBPOINTS; j++)
        {
            for (int k = 0; k < JELLO_SUBPOINTS; k++)
            {
                if (isOnSurface(i, j, k))
                {
                    Vertex vertex = {{m_jello.p[i][j][k].x, m_jello.p[i][j][k].y, m_jello.p[i][j][k].z}, black};

                    // Vertices
                    jelloVertices.push_back(vertex);

                    // Points
                    jelloIndices[0].push_back(currentIndex);

                    // Add to LUT
                    LUT[calIndex(i, j, k)] = currentIndex;

                    currentIndex++;
                }
            }
        }
    }

    auto addLine = [&](std::vector<uint16_t>& container, int i, int j, int k, int t, int u, int v) {
        if ((i >= 0 && i < JELLO_SUBPOINTS) &&
            (j >= 0 && j < JELLO_SUBPOINTS) &&
            (k >= 0 && k < JELLO_SUBPOINTS) &&
            (t >= 0 && t < JELLO_SUBPOINTS) &&
            (u >= 0 && u < JELLO_SUBPOINTS) &&
            (v >= 0 && v < JELLO_SUBPOINTS) &&
            isOnSurface(t, u, v) &&
            LUT.find(calIndex(i, j, k)) != LUT.end() &&
            LUT.find(calIndex(t, u, v)) != LUT.end())
        {
            container.push_back(LUT[calIndex(i, j, k)]);
            container.push_back(LUT[calIndex(t, u, v)]);
        }
    };

    // Structural lines
    for (int i = 0; i < JELLO_SUBPOINTS; i++)
    {
        for (int j = 0; j < JELLO_SUBPOINTS; j++)
        {
            for (int k = 0; k < JELLO_SUBPOINTS; k++)
            {
                if (isOnSurface(i, j, k))
                {
                    addLine(jelloIndices[1], i, j, k, i + 1, j, k);
                    addLine(jelloIndices[1], i, j, k, i, j + 1, k);
                    addLine(jelloIndices[1], i, j, k, i, j, k + 1);
                }
            }
        }
    }

    // Shear lines
    for (int i = 0; i < JELLO_SUBPOINTS; i++)
    {
        for (int j = 0; j < JELLO_SUBPOINTS; j++)
        {
            for (int k = 0; k < JELLO_SUBPOINTS; k++)
            {
                if (isOnSurface(i, j, k))
                {
                    addLine(jelloIndices[2], i, j, k, i + 1, j + 1, k);
                    addLine(jelloIndices[2], i, j, k, i, j + 1, k + 1);
                    addLine(jelloIndices[2], i, j, k, i + 1, j, k + 1);

                    addLine(jelloIndices[2], i + 1, j, k, i, j + 1, k);
                    addLine(jelloIndices[2], i, j + 1, k, i, j, k + 1);
                    addLine(jelloIndices[2], i, j, k + 1, i + 1, j, k);
                }
            }
        }
    }

    // Bend lines
    for (int i = 0; i < JELLO_SUBPOINTS; i++)
    {
        for (int j = 0; j < JELLO_SUBPOINTS; j++)
        {
            for (int k = 0; k < JELLO_SUBPOINTS; k++)
            {
                if (isOnSurface(i, j, k))
                {
                    addLine(jelloIndices[3], i, j, k, i + 2, j, k);
                    addLine(jelloIndices[3], i, j, k, i, j + 2, k);
                    addLine(jelloIndices[3], i, j, k, i, j, k + 2);
                }
            }
        }
    }

    m_jelloIndexBufferInfo.points.startIndex     = 0;
    m_jelloIndexBufferInfo.points.count          = jelloIndices[0].size();
    m_jelloIndexBufferInfo.structural.startIndex = m_jelloIndexBufferInfo.points.startIndex +
                                                    m_jelloIndexBufferInfo.points.count;
    m_jelloIndexBufferInfo.structural.count      = jelloIndices[1].size();
    m_jelloIndexBufferInfo.shear.startIndex      = m_jelloIndexBufferInfo.structural.startIndex +
                                                    m_jelloIndexBufferInfo.structural.count;
    m_jelloIndexBufferInfo.shear.count           = jelloIndices[2].size();
    m_jelloIndexBufferInfo.bend.startIndex       = m_jelloIndexBufferInfo.shear.startIndex +
                                                    m_jelloIndexBufferInfo.shear.count;
    m_jelloIndexBufferInfo.bend.count            = jelloIndices[3].size();

    m_jelloVertices.resize(jelloVertices.size());
    memcpy(m_jelloVertices.data(), jelloVertices.data(), jelloVertices.size() * sizeof(jelloVertices[0]));

    m_jelloIndices.clear();
    m_jelloIndices.reserve(m_jelloIndexBufferInfo.size() * sizeof(jelloIndices[0]));
    m_jelloIndices.insert(m_jelloIndices.end(), jelloIndices[0].begin(), jelloIndices[0].end());
    m_jelloIndices.insert(m_jelloIndices.end(), jelloIndices[1].begin(), jelloIndices[1].end());
    m_jelloIndices.insert(m_jelloIndices.end(), jelloIndices[2].begin(), jelloIndices[2].end());
    m_jelloIndices.insert(m_jelloIndices.end(), jelloIndices[3].begin(), jelloIndices[3].end());
}

void JelloScene::doPhysics()
{
    if (strcmp(m_jello.integrator, "RK4") == 0)
    {
        RK4(&m_jello);
    }
    else
    {
        assert(strcmp(m_jello.integrator, "Euler") == 0);
        Euler(&m_jello);
    }
}

static void framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    auto app = reinterpret_cast<JelloApp*>(glfwGetWindowUserPointer(window));
    app->setFramebufferResized(true);
    app->drawFrame();
}

JelloApp::JelloApp(char* fileName)
{
    m_pScene = new JelloScene(fileName);
    m_pScene->initVerticesAndIndices();
}

JelloApp::~JelloApp()
{
    if (m_pScene)
    {
        delete m_pScene;
        m_pScene = nullptr;
    }
}

void JelloApp::run()
{
    createWindow();
    createRenderer();
    mainLoop();
    destroyRenderer();
    destroyWindow();
}

void JelloApp::createScene(char* fileName)
{
    m_pScene = new JelloScene(fileName);
}

void JelloApp::drawFrame()
{
    m_pRenderer->render();
}

void JelloApp::setFramebufferResized(bool resized)
{
    m_pRenderer->setFramebufferResized(resized);
}

void JelloApp::createWindow()
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    m_hwindow = glfwCreateWindow(WIDTH, HEIGHT, "Jello Cube", nullptr, nullptr);
    glfwSetWindowUserPointer(m_hwindow, this);
    glfwSetFramebufferSizeCallback(m_hwindow, framebufferResizeCallback);

    glfwSetKeyCallback(m_hwindow, keyboardFunc);

    glfwSetMouseButtonCallback(m_hwindow, [](GLFWwindow* w, int button, int action, int mods) {
        double xpos, ypos;
        glfwGetCursorPos(w, &xpos, &ypos);
        mouseButton(button, action, (int)xpos, (int)ypos);
        // printf("button: %d, state: %d\n", button, action);
    });

    glfwSetCursorPosCallback(m_hwindow, [](GLFWwindow* w, double xpos, double ypos) {
        if ((glfwGetMouseButton(w, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) || (glfwGetMouseButton(w, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS))
        {
            mouseMotionDrag((int)xpos, (int)ypos);
        }
        else
        {
            mouseMotion((int)xpos, (int)ypos);
        }
    });
}

void JelloApp::createRenderer()
{
    assert(m_pRenderer == nullptr);
    m_pRenderer = new Renderer_VK(m_hwindow);

    ///@note Index buffer and vertex buffer size must be known before initializing the renderer,
    //       because the buffers will be created in the init() function of the renderer.
    m_pRenderer->updateIndexCount(m_pScene->getIndexData());
    m_pRenderer->updateVertexCount(m_pScene->getVertexData());

    // Init renderer. Allocate ncessary resources, e.g. create vertex buffer and index buffer.
    m_pRenderer->init();

    // Only update index buffer data once
    m_pRenderer->updateIndexData(m_pScene->getIndexData());

    // For rendering
    m_pRenderer->updateIndexBufferInfo(m_pScene->getIndexBufferInfo());
}

void JelloApp::mainLoop()
{
    while (!glfwWindowShouldClose(m_hwindow))
    {
        glfwPollEvents();

        if (g_iphysics && (g_istep || !g_ipause))
        {
            Sleep(10);

            m_pScene->doPhysics();

            // Update vertex buffer with new particle positions for every frame
            m_pRenderer->updateVertexData(m_pScene->getVertexData());
        }

        ///@todo change to m_pRenderer->(m_pScene)?
        drawFrame();

        g_istep = std::max(0, g_istep - 1);
    }
}

void JelloApp::destroyWindow()
{
    glfwDestroyWindow(m_hwindow);

    glfwTerminate();
}

void JelloApp::destroyRenderer()
{
    assert(m_pRenderer != nullptr);

    if (m_pRenderer)
    {
        m_pRenderer->cleanup();
        delete m_pRenderer;
        m_pRenderer = nullptr;
    }
}

#if NEW_VULKAN
int main(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("Oops! You didn't say the jello world file!\n");
        printf("Usage: %s [worldfile]\n", argv[0]);
        assert(false);
        exit(0);
    }

    JelloApp app(argv[1]);

    try
    {
        app.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
#endif // #if NEW_VULKAN

#endif // #if VULKAN_BUILD

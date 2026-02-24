#include "jelloApp.h"

#if VULKAN_BUILD
#include <cassert>
#include <cstdio>

#include <exception>
#include <iostream>

#include "input.h"
#include "renderer-vk.h"
//#include "renderer-opengl.h" // TODO: Add OpenGL renderer.

static void framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    auto app = reinterpret_cast<JelloApp*>(glfwGetWindowUserPointer(window));
    app->setFramebufferResized(true);
    app->drawFrame();
}

void JelloApp::run()
{
    createWindow();
    createRenderer();
    mainLoop();
    destroyRenderer();
    destroyWindow();
}

void JelloApp::readWorld(char* fileName)
{
    ::readWorld(fileName, &m_jello);
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
    m_pRenderer->init();
}

void JelloApp::mainLoop()
{
    while (!glfwWindowShouldClose(m_hwindow))
    {
        glfwPollEvents();
        if (g_iphysics && (g_istep || !g_ipause))
        {
            //physicsCompute();
            //particlePosUpdate();
        }
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

    JelloApp app;
    app.readWorld(argv[1]);

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

#ifndef VK_JELLO_H_
#define VK_JELLO_H_

#if _M_X64
#define NOMINMAX
#define GLFW_INCLUDE_VULKAN
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <optional>
#include <set>
#include <stdexcept>
#include <vector>
#include <array>
#include <fstream>
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include "types.h"
#include "physics.h"

#define VK_PI 3.141592653589793238462643383279

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};

const std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete()
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct Vertex
{
    glm::vec3 pos;
    glm::vec3 color;

    static VkVertexInputBindingDescription getBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        return attributeDescriptions;
    }
};

struct UniformBufferObject
{
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

class Vk_Jello
{
public:
    struct world jello;

    void run()
    {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    GLFWwindow* window;

    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkSurfaceKHR surface;

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;

    VkQueue graphicsQueue;
    VkQueue presentQueue;

    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;

    std::vector<VkImage> depthBufferImages;
    std::vector<VkDeviceMemory> depthBufferMemories;
    std::vector<VkImageView> depthBufferImageViews;
    const VkFormat depthBufferFormat = VkFormat::VK_FORMAT_D32_SFLOAT;

    VkRenderPass m_renderPass;
    VkDescriptorSetLayout m_descriptorSetLayout;
    VkPipelineLayout m_pipelineLayout;
    VkPipeline m_lineGraphicsPipeline;
    VkPipeline m_pointGraphicsPipeline;

    std::vector<VkFramebuffer> swapChainFramebuffers;

    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;

    const int MAX_FRAMES_IN_FLIGHT = 2;

    uint32_t currentFrame = 0;

    bool framebufferResized = false;

    std::vector<Vertex> m_boundingBoxVertices;
    std::vector<uint16_t> m_boundingBoxIndices;

    VkBuffer m_boundingBoxVertexBuffer;
    VkDeviceMemory m_boundingBoxVertexBufferMemory;
    VkBuffer m_boundingBoxIndexBuffer;
    VkDeviceMemory m_boundingBoxIndexBufferMemory;

    std::vector<Vertex> m_jelloVertices;
    std::vector<uint16_t> m_jelloIndices;

    struct IndexBufferInfo
    {
        size_t startIndex;
        size_t count;
    };

    struct
    {
        IndexBufferInfo points;
        IndexBufferInfo structural;
        IndexBufferInfo shear;
        IndexBufferInfo bend;
        size_t size()
        {
            return points.count + structural.count + shear.count + bend.count;
        }
    } m_jelloIndexBufferInfos;

    const glm::fvec4 k_boundingBoxColor = glm::fvec4(0.6f, 0.6f, 0.6f, 1.0f);
    const glm::fvec4 k_jelloPointColor = glm::fvec4(0.0f, 0.0f, 0.0f, 1.0f);
    const glm::fvec4 k_jelloStructuralLineColor = glm::fvec4(0.0f, 0.0f, 1.0f, 1.0f);
    const glm::fvec4 k_jelloShearLineColor = glm::fvec4(1.0f, 0.0f, 0.0f, 1.0f);
    const glm::fvec4 k_jelloBendLineColor = glm::fvec4(0.0f, 1.0f, 0.0f, 1.0f);

    // Must match push_constant struct in shader.frag
    struct PipelinePushConstantFs
    {
        glm::fvec4 color;
        bool       usePcColor;
    } pipelinePushConstantFs;

    VkBuffer m_jelloVertexBuffer;
    VkDeviceMemory m_jelloVertexBufferMemory;
    VkBuffer m_jelloIndexBuffer;
    VkDeviceMemory m_jelloIndexBufferMemory;

    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    std::vector<void*> uniformBuffersMapped;

    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;

    void initWindow();

    static void framebufferResizeCallback(
        GLFWwindow* window,
        int         width,
        int         height)
    {
        auto app =
            reinterpret_cast<Vk_Jello*>(glfwGetWindowUserPointer(window));
        app->framebufferResized = true;
        app->drawFrame();
    }

    void initVulkan();
    void mainLoop();
    void updateUniformBuffer(uint32_t currentImage);
    void physicsCompute();
    void particlePosUpdate();
    void drawFrame();
    void cleanupSwapChain();
    void destroyDepthBuffers();
    void cleanup();
    void createInstance();
    void populateDebugMessengerCreateInfo(
        VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    void setupDebugMessenger();
    void createSurface();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createSwapChain();
    void createDepthBuffers();
    void createImageViews();
    void recreateSwapChain();
    VkShaderModule createShaderModule(const std::vector<char>& code);
    void createRenderPass();
    void createDescriptorSetLayout();
    void createPipelineLayout();
    void createPointGraphicsPipeline();
    void createLineGraphicsPipeline();
    void createFramebuffers();
    void createCommandPool();
    void initBoundingBoxVertexIndexBuffers();
    void createBoundingBoxVertexBuffer();
    void createBoundingBoxIndexBuffer();
    void updateJelloVertexBuffers();
    void initJelloVertexIndexBuffers();
    void createJelloVertexBuffer();
    void createJelloIndexBuffer();
    uint32_t findMemoryType(
        uint32_t              typeFilter,
        VkMemoryPropertyFlags properties);
    void createUniformBuffers();
    void createBuffer(
        VkDeviceSize          size,
        VkBufferUsageFlags    usage,
        VkMemoryPropertyFlags properties,
        VkBuffer&             buffer,
        VkDeviceMemory&       bufferMemory);
    void copyBuffer(
        VkBuffer     srcBuffer,
        VkBuffer     dstBuffer,
        VkDeviceSize size);
    void createDescriptorPool();
    void createDescriptorSets();
    void createCommandBuffers();
    void createSyncObjects();
    void bindDynamicState(VkCommandBuffer commandBuffer);
    void recordCommandBuffer(
        VkCommandBuffer commandBuffer,
        uint32_t        imageIndex);
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(
        const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(
        const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
    bool isDeviceSuitable(VkPhysicalDevice device);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    std::vector<const char*> getRequiredExtensions();
    bool checkValidationLayerSupport();

    void DestroyDebugUtilsMessengerEXT(
        VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
        const VkAllocationCallbacks* pAllocator);
};
#endif // #if USE_GLUT
#endif // #ifndef VK_JELLO_H
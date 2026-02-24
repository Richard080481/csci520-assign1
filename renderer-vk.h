#ifndef _RENDERER_VK_H_
#define _RENDERER_VK_H_

#include <array>
#include <optional>
#include <vector>

#include <vulkan/vulkan.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "renderer.h"
#include "types.h"

#ifdef _DEBUG
const bool k_enableValidationLayers = true;
#else
const bool k_enableValidationLayers = false;
#endif

struct GLFWwindow;

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

namespace VertexHelper
{
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

// Must match push_constant struct in shader.frag
struct PipelinePushConstantFs
{
    glm::fvec4 color;
    bool usePcColor;
};

class Renderer_VK : public virtual Renderer
{
public:
    Renderer_VK(GLFWwindow* hwindow) : m_hwindow(hwindow) {}
    ~Renderer_VK() override = default;

    void init() override;
    void render() override;
    void cleanup() override;

    void setFramebufferResized(bool resized) override;
    void updateIndexBufferInfo(IndexBufferInfo indexBufferInfo) override;
    void updateIndexCount(const std::vector<uint16_t>& jelloIndices) override;
    void updateIndexData(const std::vector<uint16_t>& jelloIndices) override;
    void updateVertexCount(const std::vector<Vertex>& jelloVertices) override;
    void updateVertexData(const std::vector<Vertex>& jelloVertices) override;

  private:
    void createInstance();
    void setupDebugMessenger();
    void createSurface();

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
    bool isDeviceSuitable(VkPhysicalDevice device);
    void pickPhysicalDevice();
    void createLogicalDevice();

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    void createSwapChain();
    void recreateSwapChain();

    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void createDepthBuffers();

    void createImageViews();
    void createRenderPass();
    void createDescriptorSetLayout();
    void createPipelineLayout();

    VkShaderModule createShaderModule(const std::vector<char>& code);
    void createPointGraphicsPipeline();

    void createLineGraphicsPipeline();
    void createFramebuffers();
    void createCommandPool();
    void initBoundingBoxVertexIndexBuffers();
    void createBoundingBoxVertexBuffer();

    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    void createBoundingBoxIndexBuffer();

    void createJelloVertexBuffer();
    void createUniformBuffers();
    void createDescriptorPool();
    void createDescriptorSets();
    void createCommandBuffers();
    void createSyncObjects();

    void cleanupSwapChain();
    void destroyDepthBuffers();

    void updateUniformBuffer(uint32_t currentImage);
    void recordCommandBuffer(uint32_t currentFrame, uint32_t imageIndex);

    GLFWwindow*                     m_hwindow = nullptr;
    VkInstance                      m_instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT        m_debugMessenger = VK_NULL_HANDLE;
    VkSurfaceKHR                    m_surface = VK_NULL_HANDLE;

    VkPhysicalDevice                m_physicalDevice = VK_NULL_HANDLE;
    VkDevice                        m_device = VK_NULL_HANDLE;

    VkQueue                         m_graphicsQueue = VK_NULL_HANDLE;
    VkQueue                         m_presentQueue = VK_NULL_HANDLE;

    VkSwapchainKHR                  m_swapChain = VK_NULL_HANDLE;
    std::vector<VkImage>            m_swapChainImages;
    VkFormat                        m_swapChainImageFormat = VkFormat::VK_FORMAT_UNDEFINED;
    VkExtent2D                      m_swapChainExtent = {};
    std::vector<VkImageView>        m_swapChainImageViews;
    std::vector<VkFramebuffer>      m_swapChainFramebuffers;

    std::vector<VkImage>            m_depthBufferImages;
    std::vector<VkDeviceMemory>     m_depthBufferMemories;
    std::vector<VkImageView>        m_depthBufferImageViews;
    const VkFormat                  m_depthBufferFormat = VkFormat::VK_FORMAT_D32_SFLOAT;

    VkRenderPass                    m_renderPass = VK_NULL_HANDLE;

    VkDescriptorSetLayout           m_descriptorSetLayout = VK_NULL_HANDLE;

    PipelinePushConstantFs          m_pipelinePushConstantFs = {};
    VkPipelineLayout                m_pipelineLayout = VK_NULL_HANDLE;

    VkPipeline                      m_pointGraphicsPipeline = VK_NULL_HANDLE;
    VkPipeline                      m_lineGraphicsPipeline = VK_NULL_HANDLE;

    VkCommandPool                   m_commandPool = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer>    m_commandBuffers;

    const glm::fvec4                k_boundingBoxColor = glm::fvec4(0.6f, 0.6f, 0.6f, 1.0f);
    const glm::fvec4                k_jelloPointColor = glm::fvec4(0.0f, 0.0f, 0.0f, 1.0f);
    const glm::fvec4                k_jelloStructuralLineColor = glm::fvec4(0.0f, 0.0f, 1.0f, 1.0f);
    const glm::fvec4                k_jelloShearLineColor = glm::fvec4(0.0f, 1.0f, 0.0f, 1.0f);
    const glm::fvec4                k_jelloBendLineColor = glm::fvec4(1.0f, 0.0f, 0.0f, 1.0f);

    std::vector<Vertex>             m_boundingBoxVertices;
    std::vector<uint16_t>           m_boundingBoxIndices;

    VkBuffer                        m_boundingBoxVertexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory                  m_boundingBoxVertexBufferMemory = VK_NULL_HANDLE;
    VkBuffer                        m_boundingBoxIndexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory                  m_boundingBoxIndexBufferMemory = VK_NULL_HANDLE;

    size_t                          m_jelloVertexCount = 0;
    size_t                          m_jelloIndexCount = 0;

    IndexBufferInfo                 m_jelloIndexBufferInfo = {};
    VkBuffer                        m_jelloVertexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory                  m_jelloVertexBufferMemory = VK_NULL_HANDLE;
    VkBuffer                        m_jelloIndexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory                  m_jelloIndexBufferMemory = VK_NULL_HANDLE;

    std::vector<VkBuffer>           m_uniformBuffers;
    std::vector<VkDeviceMemory>     m_uniformBuffersMemory;
    std::vector<void*>              m_uniformBuffersMapped;

    VkDescriptorPool                m_descriptorPool = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet>    m_descriptorSets;

    std::vector<VkSemaphore>        m_imageAvailableSemaphores;
    std::vector<VkSemaphore>        m_renderFinishedSemaphores;
    std::vector<VkFence>            m_inFlightFences;

    uint32_t                        m_currentFrame = 0;
    bool                            m_framebufferResized = false;
};

#endif // #ifndef _RENDERER_VK_H_

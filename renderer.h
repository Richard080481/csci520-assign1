#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <cstdint>

#include <vector>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Vertex
{
    glm::vec3 pos;
    glm::vec3 color;
};

struct IndexBufferInfoEntry
{
    size_t startIndex;
    size_t count;
};

struct IndexBufferInfo
{
    IndexBufferInfoEntry points;
    IndexBufferInfoEntry structural;
    IndexBufferInfoEntry shear;
    IndexBufferInfoEntry bend;
    size_t size()
    {
        return points.count + structural.count + shear.count + bend.count;
    }
};

class Renderer
{
public:
    virtual ~Renderer() = default;
    virtual void init() = 0;
    virtual void render() = 0;
    virtual void cleanup() = 0;

    virtual void setFramebufferResized(bool resized) = 0;

    virtual void updateIndexBufferInfo(IndexBufferInfo indexBufferInfo) = 0;
    virtual void updateIndexCount(const std::vector<uint16_t>& jelloIndices) = 0;
    virtual void updateIndexData(const std::vector<uint16_t>& jelloIndices) = 0;
    virtual void updateVertexCount(const std::vector<Vertex>& jelloVertices) = 0;
    virtual void updateVertexData(const std::vector<Vertex>& jelloVertices) = 0;
};

#endif // #ifndef _RENDERER_H_
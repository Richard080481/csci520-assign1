#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <cstdint>

class Renderer
{
public:
    virtual ~Renderer() = default;
    virtual void init() = 0;
    virtual void render() = 0;
    virtual void cleanup() = 0;

    virtual void setFramebufferResized(bool resized) = 0;
};

#endif // #ifndef _RENDERER_H_
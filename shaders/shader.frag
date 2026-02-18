#version 450

layout(location = 0) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

layout(push_constant, std430) uniform pc
{
    vec4 color;
    bool usePcColor;
};

void main() {
    if (usePcColor != false)
    {
        outColor = color;
    }
    else
    {
        outColor = vec4(fragColor, 1.0);
    }
}
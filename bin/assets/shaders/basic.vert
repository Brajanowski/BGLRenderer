#version 330

layout(location = 0) in vec3 attribPosition;
layout(location = 1) in vec3 attribNormal;
layout(location = 3) in vec2 attribUV0;

out vec3 normal;
out vec2 uv0;

void main()
{
    normal = attribNormal;
    uv0 = attribUV0;
    gl_Position = vec4(attribPosition, 1.0);
}

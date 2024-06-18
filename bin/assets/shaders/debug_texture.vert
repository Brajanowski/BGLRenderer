#version 330

layout(location = 0) in vec3 attribPosition;
layout(location = 3) in vec2 attribUV0;

out vec2 uv0;

void main()
{
    uv0 = attribUV0;
    gl_Position = vec4(attribPosition, 1.0);
}


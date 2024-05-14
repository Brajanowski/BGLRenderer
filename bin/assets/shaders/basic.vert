#version 330

layout(location = 0) in vec2 attribPosition;

void main()
{
    gl_Position = vec4(attribPosition, 0, 1.0);
}

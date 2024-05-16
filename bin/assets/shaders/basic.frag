#version 330

in vec3 normal;
in vec2 uv0;

out vec4 fragColor;

uniform float test;

void main()
{
    fragColor = vec4(normal.x, test, uv0.y, 1);
}

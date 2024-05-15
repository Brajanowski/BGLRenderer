#version 330

in vec3 normal;
in vec2 uv0;

out vec4 fragColor;

void main()
{
    fragColor = vec4(normal, 1);
}

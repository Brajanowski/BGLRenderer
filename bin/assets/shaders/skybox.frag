#version 330

in vec2 uv0;

out vec4 fragColor;

uniform samplerCube u_texture;
uniform mat4 u_viewProjectionInv;

#include "utils.glsl"

void main()
{
    vec3 n = clipSpaceToDirection(uv0 * 2.0 - 1.0, u_viewProjectionInv);
    fragColor = texture(u_texture, n);
}


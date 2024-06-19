#version 330

in vec2 uv0;

out vec4 fragColor;

uniform sampler2D u_texture;
uniform mat4 u_viewProjectionInv;

#include "utils.glsl"

void main()
{
    vec3 direction = clipSpaceToDirection(uv0 * 2.0 - 1.0, u_viewProjectionInv);
    vec2 uv = vec2(0.5 + atan(direction.x, direction.z) / (2 * M_PI), 0.5 + asin(-direction.y) / M_PI);

    fragColor = texture2D(u_texture, uv);
}

#version 330

in vec2 uv0;

out vec4 fragColor;

uniform sampler2D u_texture;
uniform mat4 u_projectionView;
uniform mat4 u_view;
uniform mat4 u_projection;

#include "utils.glsl"

void main()
{
    vec4 clipSpace = vec4(uv0 * 2.0 - 1.0, -1.0, 1.0);

    vec4 pos = inverse(u_projection) * clipSpace;
    pos = vec4(pos.xy, -1.0, 0);

    vec3 dir = normalize((inverse(u_view) * pos).xyz);

    vec3 direction = dir;
    vec2 uv = vec2(0.5 + atan(direction.x, direction.z) / (2 * M_PI), 0.5 + asin(-direction.y) / M_PI);
    fragColor = texture2D(u_texture, uv);
}

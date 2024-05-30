#version 330

in vec2 uv0;

out vec4 fragColor;

uniform vec4 u_gridColorBackground = vec4(0.8, 0.0, 0.5, 1.0);
uniform vec4 u_gridColorForeground = vec4(1.0, 0.2, 1.0, 1.0);
uniform float u_gridSize = 100.0;

uniform vec2 u_resolution;

#include "checkerboard.glsl"

void main()
{
    vec2 pos = gl_FragCoord.xy / u_resolution;
    pos.x *= u_resolution.x / u_resolution.y;

    float t = checkerboard(pos, u_gridSize);
    fragColor = mix(u_gridColorBackground, u_gridColorForeground, t);
}
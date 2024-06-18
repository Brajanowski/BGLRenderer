#version 330

in vec2 uv0;

out vec4 fragColor;

uniform sampler2D u_texture;
uniform vec4 u_channel;

void main()
{
    vec4 color = texture2D(u_texture, uv0) * u_channel;
    float v = max(color.r, max(color.g, max(color.b, color.a)));

    fragColor = vec4(v, v, v, 1.0);
}


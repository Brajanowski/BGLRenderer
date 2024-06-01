#version 330

in vec2 uv0;

out vec4 fragColor;

uniform vec4 u_ambientColor = vec4(0.1, 0.1, 0.1, 1.0);
uniform sampler2D u_depth;

void main()
{
    float depthValue = texture2D(u_depth, uv0).r * 2.0 - 1.0;
    if (depthValue == 1.0)
    {
        discard;
    }

    fragColor = u_ambientColor;
}


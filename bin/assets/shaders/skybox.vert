#version 330

in vec2 uv0;

out vec4 fragColor;

uniform vec4 u_tint = vec4(1, 1, 1, 1); 
uniform sampler2D u_baseColor;

void main()
{
    vec4 textureColor = texture2D(u_baseColor, uv0);
    fragColor = textureColor * u_tint;
}


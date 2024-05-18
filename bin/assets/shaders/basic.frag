#version 330

in vec3 normal;
in vec2 uv0;

out vec4 fragColor;

uniform float test;
uniform sampler2D textureTest;

void main()
{
    vec4 textureColor = texture2D(textureTest, uv0);
    fragColor = vec4(textureColor.xyz * max(0.5, test), 1);
}

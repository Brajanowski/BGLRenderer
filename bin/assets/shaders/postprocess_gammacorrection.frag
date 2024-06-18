#version 330

in vec2 uv0;

out vec4 fragColor;

uniform sampler2D u_texture;

void main()
{
    vec3 color = texture2D(u_texture, uv0).xyz;

    // tonemapping
    // TODO - move tonemapping to separate post process effect
    color = color / (color + vec3(1.0));

    // gamma correction
    color = pow(color, vec3(1.0 / 2.2));

    fragColor = vec4(color, 1.0);
}



#version 330

in vec2 uv0;

out vec4 fragColor;

uniform sampler2D u_albedo;
uniform sampler2D u_light;

void main()
{
    vec3 albedo = texture2D(u_albedo, uv0).xyz;
    vec3 light = texture2D(u_light, uv0).xyz;

    fragColor = vec4(light, 1.0);
}

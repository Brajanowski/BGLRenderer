#version 330

in vec3 normal;

out vec4 fragColor;

uniform vec4 u_color = vec4(1, 1, 1, 1);

void main()
{
    vec3 ambient = u_color.xyz * 0.5;
    vec3 direction = normalize(vec3(1, 1, 0));
    float NdotL = max(dot(normal, direction), 0.0);

    fragColor = mix(vec4(ambient, u_color.a), u_color, NdotL);
}

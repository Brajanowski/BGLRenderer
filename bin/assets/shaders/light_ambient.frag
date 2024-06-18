#version 330

in vec2 uv0;

out vec4 fragColor;

uniform float u_intensity = 1.0;
uniform sampler2D u_albedo;
uniform sampler2D u_normal;
uniform sampler2D u_surface;
uniform sampler2D u_depth;
uniform samplerCube u_irradiance;

uniform vec3 u_cameraPosition;
uniform mat4 u_viewProjectionInv;

#include "utils.glsl"
#include "depth_utils.glsl"

vec3 fresnelSchlick(vec3 F0, float theta)
{
    return F0 + (vec3(1.0) - F0) * pow(clamp(1.0 - theta, 0.0, 1.0), 5.0);
}

void main()
{
    float depthValue = texture2D(u_depth, uv0).r * 2.0 - 1.0;

    vec3 worldPosition = decodeWorldPosition(depthValue, uv0 * 2.0 - 1.0, u_viewProjectionInv);
    vec3 albedo = texture2D(u_albedo, uv0).xyz;
    vec4 surfaceData = texture2D(u_surface, uv0);

    float roughness = surfaceData.x;
    float metallic = surfaceData.y;

    vec3 V = normalize(u_cameraPosition - worldPosition);
    vec3 N = normalize(texture2D(u_normal, uv0).xyz * 2.0 - 1);

    float NdotV = max(dot(N, V), 0.0001);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);
    vec3 F = fresnelSchlick(F0, NdotV);

    vec3 kD = (1.0 - F) * (1.0 - metallic);
    vec3 ambient = texture(u_irradiance, N).rgb * albedo * kD;

    fragColor = vec4(ambient, 1);
}


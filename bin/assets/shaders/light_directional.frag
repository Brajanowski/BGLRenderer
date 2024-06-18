#version 330

in vec2 uv0;

out vec4 fragColor;

uniform sampler2D u_albedo;
uniform sampler2D u_normal;
uniform sampler2D u_surface;
uniform sampler2D u_depth;
uniform sampler2D u_skyTexture;

uniform vec3 u_direction = vec3(1, -0.5, 1);
uniform vec3 u_color = vec3(1, 1, 1);
uniform float u_intensity = 1.0;

uniform vec3 u_cameraPosition;
uniform vec3 u_cameraDirection;

uniform mat4 u_viewProjectionInv;

#include "utils.glsl"
#include "depth_utils.glsl"

vec3 getSky(vec3 direction, float roughness)
{
    float lod = roughness * 4.0;
    const float skyboxGamma = 2.2;

    vec2 uv = vec2(0.5 + atan(direction.x, direction.z) / (2 * M_PI), 0.5 + asin(-direction.y) / M_PI);
    return pow(textureLod(u_skyTexture, uv, roughness * 8.0).xyz, vec3(1.0 / skyboxGamma));
}

vec3 fresnelSchlick(vec3 F0, float VdotH)
{
    return F0 + (vec3(1.0) - F0) * pow(clamp(1.0 - VdotH, 0.0, 1.0), 5.0);
}

vec3 lambertianDiffuse(vec3 color, float NdotL)
{
    return color / M_PI * max(NdotL, 0.0);
}

float distributionGGX(float alpha, float NdotH)
{
    float alpha2 = alpha * alpha;
    float NdotH2 = NdotH * NdotH;
    float nominator = alpha2;
    float denominator = (NdotH2 * (alpha2 - 1.0) + 1.0);
    denominator = M_PI * denominator * denominator;

    return nominator / denominator;
}

float geometrySchlickGGX(float roughness, float NdotV)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    float nominator = NdotV;
    float denominator = NdotV * (1.0 - k) + k;
    return nominator / denominator;
}

float geometrySmith(float roughness, vec3 N, vec3 V, vec3 L)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = geometrySchlickGGX(roughness, NdotL);
    float ggx2 = geometrySchlickGGX(roughness, NdotV);

    return ggx1 * ggx2;
}

vec3 calculateLightingPBR(vec3 light, vec3 albedo, float roughness, float metallic, vec3 V, vec3 N, vec3 L)
{
    vec3 H = normalize(V + L);
    vec3 R = normalize(reflect(-V, N));

    float NdotL = max(dot(N, L), 0.0);
    float NdotH = max(dot(N, H), 0.0);
    float NdotV = max(dot(N, V), 0.0001);
    float VdotH = max(dot(V, H), 0.0);

    float alpha = roughness * roughness;

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);
    vec3 F = fresnelSchlick(F0, VdotH);

    vec3 kD = vec3(1.0) - F;
    kD *= 1.0 - metallic;
    vec3 diffuseBRDF = kD * albedo;

    vec3 radiance = light;// * getSky(R, 1.0);

    float D = distributionGGX(alpha, NdotH);
    float G = geometrySmith(roughness, N, V, L);

    vec3 cookTorranceNominator = D * G * F;
    float cookTorranceDenominator = 4.0 * NdotL * NdotV + 0.0001;
    vec3 specularBRDF = cookTorranceNominator / cookTorranceDenominator;

    vec3 Lo = (diffuseBRDF + specularBRDF) * radiance * NdotL;
    return Lo;
}

void main()
{
    float depthValue = texture2D(u_depth, uv0).r * 2.0 - 1.0;

    if (depthValue >= 1.0)
    {
        discard;
    }

    vec3 worldPosition = decodeWorldPosition(depthValue, uv0 * 2.0 - 1.0, u_viewProjectionInv);
    vec3 surfaceColor = texture2D(u_albedo, uv0).xyz;
    vec4 surfaceData = texture2D(u_surface, uv0);

    float roughness = surfaceData.x;
    float metallic = surfaceData.y;

    vec3 V = normalize(u_cameraPosition - worldPosition);
    vec3 N = normalize(texture2D(u_normal, uv0).xyz * 2.0 - 1);
    vec3 L = normalize(-u_direction);
    
    vec3 color = calculateLightingPBR(u_color * u_intensity, surfaceColor, roughness, metallic, V, N, L);

    fragColor = vec4(color, 1.0);
}

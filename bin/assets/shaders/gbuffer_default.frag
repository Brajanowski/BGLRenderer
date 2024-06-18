#version 330

in vec3 normal;
in vec3 tangent;
in vec2 uv0;
in mat3 tbn;

layout(location = 0) out vec4 albedoBuffer;
layout(location = 1) out vec4 normalBuffer;
layout(location = 2) out vec4 surfaceBuffer;

uniform vec4 u_tint = vec4(1, 1, 1, 1); 
uniform sampler2D u_baseColor;
uniform bool u_baseColorExists;

uniform sampler2D u_normalMap;
uniform bool u_normalMapExists;

uniform float u_roughness = 0.5;
uniform sampler2D u_roughnessMap;
uniform bool u_roughnessMapExists;

uniform float u_metallic = 0.0;
uniform sampler2D u_metallicMap;
uniform bool u_metallicMapExists;

void main()
{
    vec4 albedo = u_tint;
    if (u_baseColorExists)
    {
        albedo *= texture2D(u_baseColor, uv0);
    }

    vec3 surfaceNormal = normal;
    if (u_normalMapExists)
    {
        vec3 normalMapValue = normalize(texture2D(u_normalMap, uv0).xyz * 2.0 - 1.0);
        surfaceNormal = normalize(tbn * normalMapValue);
    }

    float roughness = u_roughness;
    if (u_roughnessMapExists)
    {
        roughness = texture2D(u_roughnessMap, uv0).r;
    }

    float metallic = u_metallic;
    if (u_metallicMapExists)
    {
        metallic = texture2D(u_metallicMap, uv0).r;
    }

    albedoBuffer = albedo;
    normalBuffer = vec4(surfaceNormal * 0.5 + 0.5, 1.0);
    surfaceBuffer = vec4(roughness, metallic, 0, 0);
}

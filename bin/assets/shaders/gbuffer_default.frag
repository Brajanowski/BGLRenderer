#version 330

in vec3 normal;
in vec3 tangent;
in vec2 uv0;
in mat3 tbn;

layout(location = 0) out vec4 albedoBuffer;
layout(location = 1) out vec4 normalBuffer;

uniform vec4 u_tint = vec4(1, 1, 1, 1); 

uniform sampler2D u_baseColor;
//uniform sampler2D u_normalMap;

void main()
{
    vec4 textureColor = texture2D(u_baseColor, uv0);
    albedoBuffer = textureColor * u_tint;

    //vec3 normalMapValue = texture2D(u_normalMap, uv0).xyz * 2 - 1;
    //vec3 n = normalize(tbn * normalMapValue);
    normalBuffer = vec4(normal * 0.5 + 0.5, 1.0);
}

#version 330

layout(location = 0) in vec3 attribPosition;
layout(location = 1) in vec3 attribNormal;
layout(location = 2) in vec3 attribTangent;
layout(location = 3) in vec2 attribUV0;

out vec3 normal;
out vec3 tangent;
out vec2 uv0;
out mat3 tbn;

uniform mat4 u_model = mat4(1.0);
uniform mat4 u_viewProjection;

void main()
{
    normal = normalize(vec3(u_model * vec4(attribNormal, 0.0)));
    tangent = normalize(vec3(u_model * vec4(attribTangent, 0.0)));
    tangent = normalize(tangent - dot(tangent, normal) * normal);

    vec3 bitangent = normalize(cross(normal, tangent));
    tbn = mat3(tangent, bitangent, normal);

    uv0 = attribUV0;

    mat4 mvp = u_viewProjection * u_model;
    gl_Position = mvp * vec4(attribPosition, 1.0);
}

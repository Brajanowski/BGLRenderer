#version 330

layout (location = 0) in vec3 attribPosition;
layout (location = 1) in vec3 attribNormal;

out vec3 normal;

uniform mat4 u_model = mat4(1.0);
uniform mat4 u_viewProjection = mat4(1.0);

void main()
{
    normal = normalize(vec3(u_model * vec4(attribNormal, 0.0)));

    mat4 mvp = u_viewProjection * u_model;
    gl_Position = mvp * vec4(attribPosition, 1.0);
}

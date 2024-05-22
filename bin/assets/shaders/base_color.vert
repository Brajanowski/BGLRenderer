#version 330

layout(location = 0) in vec3 attribPosition;

uniform mat4 u_model = mat4(1.0);
uniform mat4 u_viewProjection = mat4(1.0);

void main()
{
    mat4 mvp = u_viewProjection * u_model;
    gl_Position = mvp * vec4(attribPosition, 1.0);
}

#version 330

in vec2 uv0;

out vec4 fragColor;

uniform sampler2D u_normal;
uniform sampler2D u_depth;

uniform vec3 u_direction = vec3(1, -0.5, 1);
uniform vec3 u_color = vec3(1, 1, 1);
uniform float u_intensity = 1.0;

uniform vec3 u_cameraPosition;
uniform vec3 u_cameraDirection;

uniform mat4 u_inverseViewProjection;

#include "depth_utils.glsl"
#include "lighting.glsl"

void main()
{
    float depthValue = texture2D(u_depth, uv0).r;

    vec3 lightDir = normalize(u_direction);
    vec3 worldPosition = decodeWorldPosition(depthValue * 2.0 - 1.0, uv0 * 2.0 - 1.0, u_inverseViewProjection);
    vec3 surfaceNormal = normalize(texture2D(u_normal, uv0).xyz * 2.0 - 1);
    vec3 viewDirection = normalize(u_cameraPosition - worldPosition);

    DirectionalLight light;
    light.color = u_color;
    light.intensity = u_intensity;
    light.direction = u_direction;

    SurfaceDetails surface;
    surface.color = vec3(1, 1, 1);
    surface.normal = surfaceNormal;

    ViewDetails view;
    view.direction = viewDirection;

    vec3 lighting = calculateDirectionalLight(light, surface, view);
    fragColor = vec4(lighting, 1.0);
}

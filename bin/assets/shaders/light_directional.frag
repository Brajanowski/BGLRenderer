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

vec3 decodeWorldPosition(float depthValue, vec2 clipPosition, mat4 inverseViewProjection)
{
    vec4 clipSpace;
    clipSpace.xy = clipPosition;
    clipSpace.z = depthValue;
    clipSpace.w = 1.0;

    vec4 homogeneousPosition = inverseViewProjection * clipSpace;
    return homogeneousPosition.xyz / homogeneousPosition.w;
}

void main()
{
    float depthValue = texture2D(u_depth, uv0).r;

    vec3 lightDir = normalize(u_direction);
    vec3 worldPosition = decodeWorldPosition(depthValue * 2.0 - 1.0, uv0 * 2.0 - 1.0, u_inverseViewProjection);
    vec3 surfaceNormal = normalize(texture2D(u_normal, uv0).xyz * 2.0 - 1);
    vec3 viewDirection = normalize(u_cameraPosition - worldPosition);

    vec3 lightDirReflected = reflect(lightDir, surfaceNormal);

    float spec = pow(max(0.0, dot(lightDirReflected, viewDirection)), 32);
    float specularStrength = 0.5;

    float nDotL = max(0, dot(surfaceNormal, normalize(-lightDir)));

    vec3 diffuse = u_color * u_intensity * nDotL;
    vec3 specular = vec3(spec) * specularStrength;

    fragColor = vec4(diffuse + specular, 1.0);
}

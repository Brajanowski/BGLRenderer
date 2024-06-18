#define M_PI 3.1415926535897932384626433832795

vec3 clipSpaceToDirection(vec2 clipPosition, mat4 inverseViewProjection)
{
    vec4 clipSpace;
    clipSpace.xy = clipPosition;
    clipSpace.z = 1.0;
    clipSpace.w = 1.0;

    vec4 homogeneousPosition = inverseViewProjection * clipSpace;
    return normalize(homogeneousPosition.xyz / homogeneousPosition.w);
}


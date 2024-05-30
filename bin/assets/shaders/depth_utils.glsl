vec3 decodeWorldPosition(float depthValue, vec2 clipPosition, mat4 inverseViewProjection)
{
    vec4 clipSpace;
    clipSpace.xy = clipPosition;
    clipSpace.z = depthValue;
    clipSpace.w = 1.0;

    vec4 homogeneousPosition = inverseViewProjection * clipSpace;
    return homogeneousPosition.xyz / homogeneousPosition.w;
}


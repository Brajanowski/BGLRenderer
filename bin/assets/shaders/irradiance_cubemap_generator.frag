#version 330

in vec2 uv0;

out vec4 fragColor;

uniform samplerCube u_texture;
uniform mat4 u_viewProjectionInv;

#include "utils.glsl"

void main()
{
    vec3 dir = clipSpaceToDirection(uv0 * 2.0 - 1.0, u_viewProjectionInv);

    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 right = normalize(cross(up, dir));
    up = normalize(cross(dir, right));

    vec3 irradiance = vec3(0.0);
    float sampleDelta = 0.025;
    float nrSamples = 0.0;

    for (float phi = 0.0; phi < 2.0 * M_PI; phi += sampleDelta)
    {
        for (float theta = 0.0; theta < 0.5 * M_PI; theta += sampleDelta)
        {
            vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * dir;

            // @FIXME - values are clamped to vec3(1.0) because texture is HDR texture and convolution generates ugly
            //          dotted pattern when texture contains very bright colors
            vec3 sampledValue = min(texture(u_texture, sampleVec).xyz, vec3(1.0));
            irradiance += sampledValue * cos(theta) * sin(theta);
            nrSamples++;
        }
    }
    irradiance = M_PI * irradiance * (1.0 / float(nrSamples));

    fragColor = vec4(irradiance, 1.0);
}

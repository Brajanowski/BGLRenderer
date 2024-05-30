#include "material.h"

struct DirectionalLight
{
    vec3 direction;
    vec3 color;
    float intensity;
};

struct SurfaceDetails
{
    vec3 color;
    vec3 normal;
};

struct ViewDetails
{
    vec3 direction;
};

vec3 calculateDirectionalLight(DirectionalLight light, SurfaceDetails surface, ViewDetails view)
{
    vec3 lightDirReflected = reflect(light.direction, surface.normal);

    float spec = pow(max(0.0, dot(lightDirReflected, view.direction)), 32);
    float specularStrength = 0.5;

    float nDotL = max(0, dot(surface.normal, normalize(-light.direction)));

    vec3 diffuse = light.color * light.intensity * nDotL;
    vec3 specular = vec3(spec) * specularStrength;

    return diffuse + specular;
}

#version 400

struct Material {
  vec3 diffuse;
  vec3 ambient;
  vec3 specular;
  float shininess;
};

struct BaseLight {
  vec3 color;
  float ambI;
  float difI;
};

struct DirectionalLight {
  BaseLight base;
  vec3 direction;
};

struct PointLight {
  BaseLight base;
  vec3 position;
  float far_plane;
  float constant;
  float linear;
  float quadratic;
};
// position of the camera
uniform vec3 viewPos;

// A texture is expected as program attribute
uniform bool using_textures;
uniform sampler2D texture_diffuse1;

//Materials
uniform Material material;

//Lights
#define MAX_pointLights 4
uniform PointLight PLIGHTS[MAX_pointLights];
uniform int numPointL;

#define MAX_dirLights 6
uniform DirectionalLight DLIGHTS[MAX_dirLights];
uniform int numDirL;

// Shadows
uniform sampler2DArray dirShadows;      // Combined directional shadow maps
uniform samplerCubeArray cubeShadows;  // Combined point light shadow maps

// The vertex shader will feed this input
in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;
in vec4 fragPosLSpace[MAX_dirLights];

out vec4 FragmentColor;

float calcDirShadow(int index, vec4 fpLightSpace, vec3 ldirection) {
    vec3 projCoords = fpLightSpace.xyz / fpLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    if (projCoords.z > 1.0) return 0.0;

    // Sample from the layer corresponding to the directional light index
    float closestDepth = texture(dirShadows, vec3(projCoords.xy, index)).r;
    float currentDepth = projCoords.z;
    float bias = max(0.01 * (1.0 - dot(normal, ldirection)), 0.005);
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(dirShadows, 0).xy;
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(dirShadows, vec3(projCoords.xy + vec2(x, y) * texelSize, index)).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    return shadow;
}

vec3 DirCalc(int index, vec3 objDiffuse) {
    // Normalize vectors
    DirectionalLight light = DLIGHTS[index];

    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.direction);

    // Diffuse component
    float diff = max(dot(norm, lightDir), 0.0);

    // Specular component
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // Ambient component
    vec3 ambient = light.base.ambI * material.ambient;
    vec3 diffuse = light.base.difI * diff * objDiffuse;
    vec3 specular = spec * material.specular;

    float shadow = calcDirShadow(index, fragPosLSpace[index], light.direction);
    return (ambient + (1.0 - shadow) * (diffuse + specular)) * light.base.color;
}

float calcPointShadow(vec3 fragPosition, int index) {
    vec3 fragToLight = fragPosition - PLIGHTS[index].position;

    // Sample from the layer corresponding to the point light index
    float closestDepth = texture(cubeShadows, vec4(fragToLight, index)).r;
    closestDepth *= PLIGHTS[index].far_plane;

    float currentDepth = length(fragToLight);
    float bias = 0.05;
    return currentDepth - bias > closestDepth ? 1.0 : 0.0;
}

vec3 PointCalc(int index, vec3 objDiffuse) {
    PointLight light = PLIGHTS[0];

    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position - fragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient = light.base.ambI * material.ambient;
    vec3 diffuse = light.base.difI * diff * objDiffuse;
    vec3 specular = spec * material.specular;

    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    float shadow = calcPointShadow(fragPos, 0);
    return (ambient + (1.0 - shadow) * (diffuse + specular)) * light.base.color;
}

void main() {

    vec3 object_color = material.diffuse;
    if (using_textures) {
        object_color = texture(texture_diffuse1, texCoord).rgb;
    }

    vec3 light = vec3(0.0);
    for (int i = 0; i < numDirL; i++) {
        light += DirCalc(i, object_color);
    }
//    for (int i = 0; i < numPointL; i++) {
//        light += PointCalc(i, object_color);
//    }

    FragmentColor = vec4(light * object_color, 1.0);
}

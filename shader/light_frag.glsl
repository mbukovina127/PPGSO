#version 330

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

//Shadows
uniform sampler2D dirShadows[MAX_dirLights];
uniform samplerCube cubeShadows[2];


// The vertex shader will feed this input
in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;
in vec4 fragPosLSpace[MAX_dirLights];

out vec4 FragmentColor;

float calcDirShadow(sampler2D shadowMap, vec4 fpLightSpace, vec3 ldirection) {
    vec3 projCoords = fpLightSpace.xyz / fpLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    if (projCoords.z > 1.0) return 0.0;

    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float bias = max(0.01 * (1.0 - dot(normal, ldirection)), 0.005);
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
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

    float shadow = calcDirShadow(dirShadows[index], fragPosLSpace[index], light.direction);
    return (ambient + (1.0 - shadow) * (diffuse + specular)) * light.base.color;
}
float calcPointShadow(vec3 fragPosition, vec3 lightPos, float far_plane, int index) {
    vec3 fragToLight = fragPosition - lightPos;
    float closestDepth = texture(cubeShadows[index], fragToLight).r;
    closestDepth *= far_plane;

    float currentDepth = length(fragToLight);
    float bias = 0.05;
    return currentDepth -  bias > closestDepth ? 1.0 : 0.0;
}
vec3 PointCalc(PointLight light, vec3 objDiffuse, int index) {
    // Normalize vectors
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position);

    // Diffuse component
    float diff = max(dot(norm, lightDir), 0.0);

    // Specular component
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient = light.base.ambI * material.ambient;
    vec3 diffuse = light.base.difI * diff * objDiffuse;
    vec3 specular = spec * material.specular;

    // length of the vector
    float distance = length(light.position - fragPos);

    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance*distance));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    float shadow = calcPointShadow(fragPos, light.position, light.far_plane, index);
    return (ambient + (1.0 - shadow) * (diffuse + specular)) * light.base.color;
}
void main() {

    vec3 object_color = material.diffuse;
    if (using_textures) {
      object_color = texture(texture_diffuse1, texCoord).rgb;
    }
    vec3 light = vec3(0.0);
//    for (int i = 0; i < numDirL; i++) {
//        light += DirCalc(i, object_color);
//    }
    for (int i = 0; i < numPointL; i++) {
        light += PointCalc(PLIGHTS[i], object_color, i);
    }

//    vec3 fragToLight = fragPos - PLIGHTS[0].position;
//    float closestDepth = texture(cubeShadows[0], fragToLight).r;

//    FragmentColor = vec4(vec3(closestDepth * 10.f) ,1);
    FragmentColor = vec4(light * object_color ,1);
}

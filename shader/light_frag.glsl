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

#define MAX_dirLights 10
uniform DirectionalLight DLIGHTS[MAX_dirLights];
uniform int numDirL;

// The vertex shader will feed this input
in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;
out vec4 FragmentColor;


vec3 DirCalc(DirectionalLight light, vec3 objDiffuse) {
    // Normalize vectors
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

    // Combine all lighting contributions
    return (ambient + diffuse + specular) * light.base.color;
}

vec3 PointCalc(PointLight light, vec3 objDiffuse) {
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

    return (ambient + diffuse + specular) * light.base.color;
}
void main() {

    vec3 object_color = material.diffuse;
    if (using_textures) {
      object_color = texture(texture_diffuse1, texCoord).rgb;
    }
    vec3 light = vec3(0.0);
    for (int i = 0; i < numDirL; i++) {
//        light += DirCalc(DLIGHTS[i], object_color);
    }
    for (int i = 0; i < numPointL; i++) {
        light += PointCalc(PLIGHTS[i], object_color);
    }

    FragmentColor = vec4(light * object_color ,1);
}

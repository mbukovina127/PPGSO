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

//struct DirectionalLight {
//  BaseLight base;
//  vec3 direction;
//};

struct PointLight {
  BaseLight base;
  vec3 position;
//  float constant;
//  float linear;
//  float quadratic;
};
// position of the camera
uniform vec3 viewPos;

// A texture is expected as program attribute
uniform bool using_textures;
uniform sampler2D texture_diffuse1;

//Materials
uniform Material material;

//Lights
uniform PointLight light;

// The vertex shader will feed this input
in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;
out vec4 FragmentColor;


vec3 DirectLight(vec3 objDiffuse) {
    // Normalize vectors
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position - fragPos);

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
void main() {

  vec3 object_color = material.diffuse;
  if (using_textures) {
      object_color = texture(texture_diffuse1, texCoord).rgb;
  }
    vec3 light = DirectLight(object_color);
  FragmentColor = vec4(light * object_color ,1);
}

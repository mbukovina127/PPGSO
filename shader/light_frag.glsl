#version 330

struct Material {
  vec3 diffuse;
  vec3 ambient;
  vec3 specular;
  float shininess;
};

//struct BaseLight {
//  vec3 color;
//  float ambientIntensity;
//  float diffuseIntensity;
//};

//struct DirectionalLight {
//  BaseLight base;
//  vec3 direction;
//};

struct PointLight {
//  BaseLight base;
  vec3 color;
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

// Wordspace normal passed from vertex shader
in vec3 normal;

//
in vec3 fragmentPosition;

// The final color
out vec4 FragmentColor;

void main() {

  vec3 object_color = vec3(material.diffuse);
//  if (using_textures) {
//    object_color = texture(texture_diffuse1, texCoord);
//  } else {
//  }

  vec3 ambient = material.ambient * light.color;

  // diffuse
  vec3 norm = normalize(normal);
  vec3 lightDir = normalize(light.position - fragmentPosition);
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = diff * light.color;

  // specular
  vec3 viewDir = normalize(viewPos - fragmentPosition);
  vec3 reflectDir = reflect(-lightDir, norm);
  vec3 specular = material.specular * material.shininess * light.color;

  vec3 result = (ambient + diffuse + specular) * object_color;
  FragmentColor = vec4(result,1);
//  //this will probably fuck things up
//    vec4 result = vec4(ambient + diffuse + specular, 1) * object_color;
//    FragmentColor = result;
}

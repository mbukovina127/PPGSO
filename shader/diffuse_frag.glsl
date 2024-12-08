#version 330

struct Material {
  vec3 diffuse;
  vec3 ambient;
  vec3 specular;
  float shininess;
};

// A texture is expected as program attribute
uniform bool using_textures;
uniform sampler2D texture_diffuse1;

//Materials
uniform Material material;

//// Direction of light
//uniform vec3 LightDirection;
//
//// (optional) Transparency
//uniform float Transparency;
//
//// (optional) Texture offset
//uniform vec2 TextureOffset;

// The vertex shader will feed this input
in vec2 texCoord;

// Wordspace normal passed from vertex shader
in vec4 normal;

// The final color
out vec4 FragmentColor;

void main() {

  vec4 color = vec4(0);
  if (using_textures) {
    FragmentColor = texture(texture_diffuse1, texCoord);
  } else {
    color = vec4(material.diffuse, 1);
    FragmentColor = color;
  }
}

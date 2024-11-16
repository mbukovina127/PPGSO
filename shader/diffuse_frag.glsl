#version 330
// A texture is expected as program attribute
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_ao1;


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
  // Compute diffuse lighting
//  float diffuse = max(dot(normal, vec4(normalize(LightDirection), 1.0f)), 0.0f);

  // Lookup the color in Texture on coordinates given by texCoord
  // NOTE: Texture coordinate is inverted vertically for compatibility with OBJ
  vec4 diff = texture(texture_diffuse1, texCoord);
  vec4 ao = texture(texture_ao1, texCoord);

  FragmentColor = diff  * ao;
//  FragmentColor.a = Transparency;
}

#version 330 core

layout (location = 0) in vec3 aPos;      // Vertex position
layout (location = 1) in vec3 aNormal;   // Vertex normal
layout (location = 2) in vec2 aTexCoord; // Texture coordinates
layout (location = 3) in mat4 instanceModel; // Instance transformation matrix

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 model;

out vec3 FragPos;      // Position of the vertex in world space
out vec3 Normal;       // Normal vector
out vec2 TexCoord;     // Texture coordinates

void main() {
  // Compute the model-view-projection position for the instance
  mat4 ModelMatrix = instanceModel * model; // Instance-specific transformation
  gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(aPos, 1.0);

  // Pass data to the fragment shader
  FragPos = vec3(ModelMatrix * vec4(aPos, 1.0)); // Position in world space
  Normal = mat3(transpose(inverse(ModelMatrix))) * aNormal; // Correct normal
  TexCoord = aTexCoord;
}

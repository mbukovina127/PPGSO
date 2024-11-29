#version 330
// The inputs will be fed by the vertex buffer objects
layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 TexCoord;
// Matrices as program attributes
uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;
uniform mat4 lSpaceMatrix;
// This will be passed to the fragment shader
out vec2 texCoord;

// Normal to pass to the fragment shader
out vec3 normal;

// fragment posititon
out vec3 fragPos;

out vec4 fragPosLSpace;

void main() {
  // Copy the input to the fragment shader
  fragPos = vec3(ModelMatrix * vec4(Position, 1.0));
  texCoord = TexCoord;

  // Normal in world coordinates updated for scaling / lighting
  normal = mat3(transpose(inverse(ModelMatrix))) * Normal;

  fragPosLSpace = lSpaceMatrix * vec4(fragPos, 1.0);

  // Calculate the final position on screen
  gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(Position, 1.0);
}

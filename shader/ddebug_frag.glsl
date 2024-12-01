#version 400 core
out vec4 FragColor;

in vec2 TexCoords;

//shadow maps;
uniform sampler2DArray depthMap;
uniform float near_plane;
uniform float far_plane;

// cubemaps
uniform samplerCubeArray cubeShadows; // Shadow cubemap array
uniform int lightIndex;              // Index of the light whose shadow to view
uniform int faceIndex;               // Index of the face to visualize

// required when using a perspective projection matrix
float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}

void main()
{
    float depthValue = texture(depthMap, vec3(TexCoords.xy, 0)).r;
    // FragColor = vec4(vec3(LinearizeDepth(depthValue) / far_plane), 1.0); // perspective
    FragColor = vec4(vec3(depthValue / 25.f), 1.0); // orthographic
}
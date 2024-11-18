//
// Created by marko on 12. 11. 2024.
//

#ifndef CAMERA_H
#define CAMERA_H
#include <ppgso.h>
#include <glm/detail/type_mat.hpp>
#include <glm/detail/type_vec.hpp>

glm::vec3 cameraPostion{2,0,2};
glm::vec3 cameraFront{0,0,0};
glm::vec3 cameraUp{0,1,0};
bool firstMouse = true;
float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch =  0.0f;
float lastX =  800.0f / 2.0;
float lastY =  600.0 / 2.0;
float fov   =  45.0f;

class Camera {
public:

 glm::mat4 viewMatrix;
 glm::mat4 projectionMatrix;


 /*!
  * Create new Camera that will generate viewMatrix and projectionMatrix based on its position, up and back vectors
  * @param fow - Field of view in degrees
  * @param ratio - Viewport screen ratio (usually width/height of the render window)
  * @param near - Distance to the near frustum plane
  * @param far - Distance to the far frustum plane
  */
 Camera(float fow = 60.0f, float ratio = 1.7f, float near = 0.1f, float far = 100.0f) {
  float fowInRad = (ppgso::PI/180.0f) * fow;

  projectionMatrix = glm::perspective(fowInRad, ratio, near, far);
 }

 /*!
  * Update Camera viewMatrix based on up, position and back vectors
  */
 void update() {
   viewMatrix = glm::lookAt(cameraPostion, cameraPostion + cameraFront, cameraUp);
 }

 /*!
  * Get direction vector in world coordinates through camera projection plane
  * @param u - camera projection plane horizontal coordinate [-1,1]
  * @param v - camera projection plane vertical coordinate [-1,1]
  * @return Normalized vector from camera position to position on the camera projection plane
  */
 glm::vec3 cast(double u, double v) {
  // Create point in Screen coordinates
  glm::vec4 screenPosition{u,v,0.0f,1.0f};

  // Use inverse matrices to get the point in world coordinates
  auto invProjection = glm::inverse(projectionMatrix);
  auto invView = glm::inverse(viewMatrix);

  // Compute position on the camera plane
  auto planePosition = invView * invProjection * screenPosition;
  planePosition /= planePosition.w;

  // Create direction vector
  auto direction = glm::normalize(planePosition - glm::vec4{cameraPostion,1.0f});
  return glm::vec3{direction};
 }
};
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
 float xpos = static_cast<float>(xposIn);
 float ypos = static_cast<float>(yposIn);

 if (firstMouse)
 {
  lastX = xpos;
  lastY = ypos;
  firstMouse = false;
 }

 float xoffset = xpos - lastX;
 float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
 lastX = xpos;
 lastY = ypos;

 float sensitivity = 0.1f; // change this value to your liking
 xoffset *= sensitivity;
 yoffset *= sensitivity;

 yaw += xoffset;
 pitch += yoffset;

 // make sure that when pitch is out of bounds, screen doesn't get flipped
 if (pitch > 89.0f)
  pitch = 89.0f;
 if (pitch < -89.0f)
  pitch = -89.0f;

 glm::vec3 front;
 front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
 front.y = sin(glm::radians(pitch));
 front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
 cameraFront = glm::normalize(front);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
 fov -= (float)yoffset;
 if (fov < 1.0f)
  fov = 1.0f;
 if (fov > 45.0f)
  fov = 45.0f;
}
#endif //CAMERA_H

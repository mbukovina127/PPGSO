//
// Created by marko on 12. 11. 2024.
//

#ifndef CAMERA_H
#define CAMERA_H
#include <ppgso.h>
#include <glm/detail/type_mat.hpp>
#include <glm/detail/type_vec.hpp>

class Camera {
public:
    glm::vec3 up{0,1,0};
    glm::vec3 position{2,0,2};
    glm::vec3 center{0,0,0};

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
      viewMatrix = glm::lookAt(position, center, up);
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
     auto direction = glm::normalize(planePosition - glm::vec4{position,1.0f});
     return glm::vec3{direction};
    }
};

#endif //CAMERA_H

//
// Created by marko on 8. 12. 2024.
//

#ifndef ANICAM_H
#define ANICAM_H
#include <ppgso.h>
#include <glm/detail/type_mat.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct cam_keyframe {
    float time;
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;

    cam_keyframe(float t, glm::vec3 pos, glm::vec3 tar, glm::vec3 up = {0,1,0})
        : time(t), position(pos), target(tar), up(up) {}
};

class Anicam {
public:

    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;

    std::vector<cam_keyframe> keyframes;  // Store keyframes for animation

    glm::vec3 position = {0, 1 ,0};
    glm::vec3 target = {0,0,0};
    glm::vec3 up = {0, 1, 0};

    /*!
     * Create new Camera that will generate viewMatrix and projectionMatrix based on its position, up and back vectors
     * @param fow - Field of view in degrees
     * @param ratio - Viewport screen ratio (usually width/height of the render window)
     * @param near - Distance to the near frustum plane
     * @param far - Distance to the far frustum plane
     */

    Anicam(float fow = 60.0f, float ratio = 1.7f, float near = 0.1f, float far = 100.0f) {
        float fowInRad = (ppgso::PI/180.0f) * fow;

        projectionMatrix = glm::perspective(fowInRad, ratio, near, far);
    }

    float animationTime = 0.0f;      // Track the animation time

    void interpolateKeyframes(float dt) {
        animationTime += dt;

        if (keyframes.size() < 2) return;

        // Find the current keyframe pair
        cam_keyframe* prev = nullptr;
        cam_keyframe* next = nullptr;
        for (size_t i = 0; i < keyframes.size() - 1; ++i) {
            if (animationTime >= keyframes[i].time && animationTime <= keyframes[i + 1].time) {
                prev = &keyframes[i];
                next = &keyframes[i + 1];
                break;
            }
        }

        if (!prev || !next) return;

        // Interpolate based on time
        float t = (animationTime - prev->time) / (next->time - prev->time);
        position = glm::mix(prev->position, next->position, t);
        target = glm::mix(prev->target, next->target, t);
        up = glm::mix(prev->up, next->up, t);
    }

    /*!
     * Update Camera viewMatrix based on up, position and back vectors
     */
    void update(float dt) {
        interpolateKeyframes(dt);
        viewMatrix = glm::lookAt(position, target, up);
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
#endif //ANICAM_H

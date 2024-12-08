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

    std::vector<cam_keyframe> keyframe_position;  // Store keyframes for animation
    std::vector<cam_keyframe> keyframe_target;  // Store keyframes for animation

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

        if (keyframe_position.size() < 2) return;

        // Find the current keyframe pair
        cam_keyframe* prev_p = nullptr;
        cam_keyframe* next_p = nullptr;
        for (size_t i = 0; i < keyframe_position.size() - 1; ++i) {
            if (animationTime >= keyframe_position[i].time && animationTime <= keyframe_position[i + 1].time) {
                prev_p = &keyframe_position[i];
                next_p = &keyframe_position[i + 1];
                break;
            }
        }

        cam_keyframe* prev_t = nullptr;
        cam_keyframe* next_t = nullptr;
        for (size_t i = 0; i < keyframe_target.size() - 1; ++i) {
            if (animationTime >= keyframe_target[i].time && animationTime <= keyframe_target[i + 1].time) {
                prev_t = &keyframe_target[i];
                next_t = &keyframe_target[i + 1];
                break;
            }
        }

        if (prev_p && next_p) {
            float t = (animationTime - prev_p->time) / (next_p->time - prev_p->time);
            position = glm::mix(prev_p->position, next_p->position, t);
        }
        if (prev_t && next_t) {
            float t = (animationTime - prev_t->time) / (next_t->time - prev_t->time);
            target = glm::mix(prev_t->target, next_t->target, t);
        }
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

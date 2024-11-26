//
// Created by Iveta Leskova on 26/11/2024.
//

#ifndef AXIS_H
#define AXIS_H
// Axis.h
#pragma once

#include <ppgso/ppgso.h>

/*!
 * Axis class for rendering colorful XYZ axes
 */
class Axis {
public:
    /*!
     * Initialize the axis geometry and shaders
     */
    Axis() {
        // Define vertices for the axes
        std::vector<ppgso::Vertex> vertices = {
            // X axis - Red
            {{0, 0, 0}, {1, 0, 0}, {1, 0, 0}},
            {{1, 0, 0}, {1, 0, 0}, {1, 0, 0}},

            // Y axis - Green
            {{0, 0, 0}, {0, 1, 0}, {0, 1, 0}},
            {{0, 1, 0}, {0, 1, 0}, {0, 1, 0}},

            // Z axis - Blue
            {{0, 0, 0}, {0, 0, 1}, {0, 0, 1}},
            {{0, 0, 1}, {0, 0, 1}, {0, 0, 1}},
        };

        // Load data into buffers
        mesh = std::make_unique<ppgso::Mesh>(vertices, GL_LINES);
    }

    /*!
     * Render the axes
     */
    void render(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix) {
        // Set up the shader
        static auto shader = ppgso::Shader{"ppgso/shaders/color.vert", "ppgso/shaders/color.frag"};

        shader.use();
        shader.setUniform("ModelMatrix", glm::mat4{1.0f}); // Identity matrix
        shader.setUniform("ViewMatrix", viewMatrix);
        shader.setUniform("ProjectionMatrix", projectionMatrix);

        // Render the mesh
        mesh->render();
    }

private:
    std::unique_ptr<ppgso::Mesh> mesh;
};


#endif //AXIS_H

//
// Created by marko on 17. 11. 2024.
//

#ifndef CHAIR_H
#define CHAIR_H
#include "scene.h"
#include "model.h"
#include <ppgso/shader.h>

class Chair : public Model {
public:
    Chair(string const &path, bool gamma = false) : Model(path, gamma) {}
    float fulltime = 0;
    bool update(Scene &scene, float dt) override {
        fulltime += dt;
        position.x = sin(fulltime);
        modelMatrix = glm::translate(glm::mat4(1.0f), position)
        // * glm::rotate( glm::mat4(1.0f), glm::radians(sin(dt)), glm::vec3(0, 1, 0))
        * glm::orientate4(rotation)
        * glm::scale(glm::mat4(1.0f), scale);
        return true;
    }

    void render (Scene &scene) override {
        // Generate the model matrix from position, rotation, and scale
        generateModelMatrix();
        // Pass the model matrix to the shader
        scene.shader->setUniform("ModelMatrix", modelMatrix);

        // Render each mesh
        for (auto &mesh : meshes) {
            mesh.render(*scene.shader);
        }
        for (auto &child : children) {
            child->render(scene);
        }
    }
};


#endif //CHAIR_H

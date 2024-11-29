//
// Created by marko on 19. 11. 2024.
//

#ifndef LAMP_H
#define LAMP_H
#include <string>

#include "src/project/scene.h"


class Lamp : public Model {
public:
    Lamp(std::string const &path, bool gamma = false) : Model(path, gamma) {}

    bool update(Scene &scene, float dt) override {
        return true;
    }

    void render (Scene &scene) override {
        // Generate the model matrix from position, rotation, and scale
        generateModelMatrix();
        // Pass the model matrix to the shader
        scene.shader.setUniform("ModelMatrix", modelMatrix);

        // Render each mesh
        for (auto &mesh : meshes) {
            mesh.render(scene.shader);
        }
        for (auto &child : children) {
            child->render(scene);
        }
    }
};

#endif //LAMP_H

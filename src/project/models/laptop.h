//
// Created by Iveta Leskova on 26/11/2024.
//

#ifndef LAPTOP_H
#define LAPTOP_H
#include "src/project/model.h"
#include "src/project/scene.h"


class Laptop : public Model {
public:
    Laptop(string const &path, bool gamma = false) : Model(path, gamma) {}
    float fulltime = 0;

    bool update(Scene &scene, float dt) override {
        for (auto &child : children) {
            child->update(scene, dt);
        }
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

    void renderDepth(ppgso::Shader &shader) override {
        generateModelMatrix();
        shader.setUniform("model", modelMatrix);

        for ( auto& mesh : meshes )
            mesh.renderDepth(shader);
        for ( auto& child : children )
            child->renderDepth(shader);
    }
};

#endif //LAPTOP_H

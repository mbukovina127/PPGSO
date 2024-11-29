//
// Created by Iveta Leskova on 29/11/2024.
//

#ifndef ROOMBA_H
#define ROOMBA_H
#include "src/project/model.h"
#include "src/project/scene.h"


class Roomba : public Model {
public:
    Roomba(string const &path, bool gamma = false) : Model(path, gamma) {}
    float fulltime = 0;

    bool update(Scene &scene, float dt) override {
        interpolateKeyframes(dt);  // Update transformations based on keyframes


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
};

#endif //ROOMBA_H

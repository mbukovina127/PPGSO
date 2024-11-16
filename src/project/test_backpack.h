//
// Created by marko on 15. 11. 2024.
//

#ifndef TEST_BACKPACK_H
#define TEST_BACKPACK_H
#include "model.h"
#include "scene.h"
#include <ppgso/shader.h>

class BPACK : public Model {
public:
    BPACK(string const &path, bool gamma = false) : Model(path, gamma) {}

    bool update(Scene &scene, float dt) override {
        return true;
    }
    void render (Scene &scene) override {
        // Generate the model matrix from position, rotation, and scale
        generateModelMatrix();

        // Use a shader program (assuming scene has a shader or it's passed in some other way)

        // Pass the model matrix to the shader
        scene.shader->use();
        scene.shader->setUniform("ModelMatrix", modelMatrix);

        // Render each mesh
        for (auto &mesh : meshes) {
            mesh.render(*scene.shader);
        }
    }
};

#endif //TEST_BACKPACK_H

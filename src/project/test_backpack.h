//
// Created by marko on 15. 11. 2024.
//

#ifndef TEST_BACKPACK_H
#define TEST_BACKPACK_H
#include "model.h"
#include "scene.h"

class BPACK : public Model {
public:
    BPACK(string const &path, bool gamma = false) : Model(path, gamma) {}
    bool update(Scene &scene, float dt) override {
        return true;
    }
    void render (Scene &scene) override
    {
        // Set up light
        // shader->setUniform("LightDirection", scene.lightDirection);

        // use camera
        // scene.shader->setUniform("ProjectionMatrix", scene.camera->projectionMatrix);
        // scene.shader->setUniform("ViewMatrix", scene.camera->viewMatrix);

        // render mesh
        scene.shader->setUniform("ModelMatrix", modelMatrix);

        for (auto mesh: meshes) {
            mesh.render(*scene.shader);
        }
    }
};

#endif //TEST_BACKPACK_H

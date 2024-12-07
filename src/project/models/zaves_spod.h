//
// Created by Iveta Leskova on 07/12/2024.
//

#ifndef ZAVES_SPOD_H
#define ZAVES_SPOD_H

#include "../scene.h"
#include "../model.h"
#include <ppgso/shader.h>

class Zaves : public Model {
public:
    Zaves(string const &path, bool gamma = false) : Model(path, gamma) {}
    float fulltime = 0;

    bool update(Scene &scene, float dt) override {
        fulltime += dt / 3;

        if (fulltime < 4){
            return true;
        }

        rotation.x = -abs(sin(fulltime - 4) * sin((fulltime - 4)/4) * sin((fulltime - 4) * 3)) /5;

        for (auto &child : children) {
            child->update(scene, dt);
        }

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

    void renderDepth(ppgso::Shader &shader) override {
        generateModelMatrix();
        shader.setUniform("model", modelMatrix);

        for ( auto& mesh : meshes )
            mesh.renderDepth(shader);
        for ( auto& child : children )
            child->renderDepth(shader);
    }
};

#endif //ZAVES_SPOD_H

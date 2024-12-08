//
// Created by marko on 8. 12. 2024.
//

#ifndef ANIMATED_H
#define ANIMATED_H
#include "../scene.h"
#include "../model.h"
#include <ppgso/shader.h>

class Animated : public Model {
public:
    Animated(string const &path, bool gamma = false) : Model(path, gamma) {}
    float fulltime = 0;

    bool update(Scene &scene, float dt) override {
        interpolateKeyframes(dt);  // proceduralne animovanie???


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

#endif //ANIMATED_H

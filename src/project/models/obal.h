//
// Created by Iveta Leskova on 07/12/2024.
//

#ifndef OBAL_H
#define OBAL_H

#include "../scene.h"
#include "../model.h"
#include <ppgso/shader.h>

class Obal : public Model {
public:
    Obal(string const &path, float & rot, bool gamma = false) : Model(path, gamma), zaves(rot)
    {
    }

    float fulltime = 0;
    float &zaves;

    bool update(Scene &scene, float dt) override {
        fulltime = zaves - 1;

        if (fulltime < 4){
            return true;
        }

        rotation.x = -abs(sin(fulltime - 4) * sin((fulltime - 4)/4) * sin((fulltime - 4) * 3)) /2;


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

#endif //OBAL_H

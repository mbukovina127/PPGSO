//
// Created by marko on 18. 11. 2024.
//

#ifndef ROOM_H
#define ROOM_H

#include "../scene.h"
#include "../model.h"
#include <ppgso/shader.h>

class Room : public Model {
public:
    Room(string const &path, bool gamma = false) : Model(path, gamma) {}

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
};
#endif //ROOM_H

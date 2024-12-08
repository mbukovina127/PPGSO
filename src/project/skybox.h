//
// Created by TINKPAD on 8. 12. 2024.
//

#ifndef SKYBOX_H
#define SKYBOX_H

#include <string>

#include "src/project/model.h"
#include <shaders/diffuse_vert_glsl.h>
#include <shaders/diffuse_frag_glsl.h>

class Scene;

class Skybox : public Model {
public:
    ppgso::Shader skyboxShader = ppgso::Shader(diffuse_vert_glsl, diffuse_frag_glsl, "");
    Skybox(std::string const &path, bool gamma = false) : Model(path, gamma) {
        position = glm::vec3(0, -7.5f,0);
        rotation = glm::vec3(0, glm::radians(180.f), 0);
    }

    bool update(Scene &scene, float dt) override {
        return true;
    }

    void render (glm::mat4 proj, glm::mat4 view) {
        generateModelMatrix();
        skyboxShader.use();
        skyboxShader.setUniform("ProjectionMatrix", proj);
        skyboxShader.setUniform("ViewMatrix", view);
        skyboxShader.setUniform("ModelMatrix", modelMatrix);
        for (auto &mesh : meshes) {
            mesh.render(skyboxShader);
        }
    }

    void render (Scene &scene) override {
        return;
    }

    void renderDepth(ppgso::Shader &shader) override {
        return;
    }
};


#endif //SKYBOX_H

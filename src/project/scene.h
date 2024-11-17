#ifndef SCENE_H
#define SCENE_H
#include <memory>
#include <vector>

#include "camera.h"
#include "model.h"

#include <shaders/diffuse_vert_glsl.h>
#include <shaders/diffuse_frag_glsl.h>


class Scene {
public:
    std::unique_ptr<ppgso::Shader> shader = make_unique<ppgso::Shader>(diffuse_vert_glsl, diffuse_frag_glsl);
    std::unique_ptr<Camera> camera = std::make_unique<Camera>();
    std::vector<std::unique_ptr<Model>> models;

    void update(float time) {
        camera->update();

        // Use iterator to update all objects so we can remove while iterating
        auto i = std::begin(models);

        while (i != std::end(models)) {
            // Update and remove from list if needed
            auto model = i->get();
            if (!model->update(*this, time))
                i = models.erase(i); // NOTE: no need to call destructors as we store shared pointers in the scene
            else
                ++i;
        }
    };

    void render() {

        shader->use();
        shader->setUniform("ProjectionMatrix", camera->projectionMatrix);
        shader->setUniform("ViewMatrix", camera->viewMatrix);

        for ( auto& model : models )
            model->render(*this);
    }

};

#endif //SCENE_H


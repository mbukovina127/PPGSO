//
// Created by marko on 11. 11. 2024.
//

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
    std::vector< std::unique_ptr<Model>> objects;

    void update(float time) {
        camera->update();

        // Use iterator to update all objects so we can remove while iterating
        auto i = std::begin(objects);

        while (i != std::end(objects)) {
            // Update and remove from list if needed
            auto obj = i->get();
            if (!obj->update(*this, time))
                i = objects.erase(i); // NOTE: no need to call destructors as we store shared pointers in the scene
            else
                ++i;
        }
    };

    void render() {
        for ( auto& obj : objects )
            obj->render(*this);
    }

};

#endif //SCENE_H


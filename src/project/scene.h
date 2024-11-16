//
// Created by marko on 11. 11. 2024.
//

#ifndef SCENE_H
#define SCENE_H
#include <memory>
#include <vector>

#include "camera.h"
#include "model.h"
#include <shaders/diffuse_frag_glsl.h>
#include <shaders/diffuse_vert_glsl.h>
#include <shaders/color_vert_glsl.h>
#include <shaders/color_frag_glsl.h>



class Scene {
public:
    std::unique_ptr<ppgso::Shader> shader = make_unique<ppgso::Shader>(color_vert_glsl, color_frag_glsl);
    std::unique_ptr<Camera> camera = std::make_unique<Camera>();
    std::vector< std::unique_ptr<Model>> objects;

    void update(float time);

    void render();

};

#endif //SCENE_H


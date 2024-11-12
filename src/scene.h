//
// Created by marko on 11. 11. 2024.
//

#ifndef SCENE_H
#define SCENE_H
#include <memory>
#include <vector>

#include "camera.h"
#include "Object.h"

class Scene {
public:

    void update(float time);

    void render();

    std::unique_ptr<Camera> camera;

    std::vector< std::unique_ptr<Object>> objects;

};

#endif //SCENE_H


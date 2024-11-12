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

    void update(float time)
    {
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
    } ;

    void render()
    {
        for ( auto& obj : objects )
            obj->render(*this);
    };

    std::unique_ptr<Camera> camera;

    std::vector< std::unique_ptr<Object>> objects;

};

#endif //SCENE_H


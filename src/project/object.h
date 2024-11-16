#pragma once

#ifndef OBJECT_H
#define OBJECT_H
#include <memory>
#include <vector>
#include "model.h"
// Forward declare a scene
class Scene;

class Object {
public:


    std::vector<std::unique_ptr<Object>> children;
    std::unique_ptr<Object> parent;

    virtual bool update(Scene &scene, float dt) = 0;

    virtual void render(Scene &scene) = 0;
private:

};

#endif //OBJECT_H

//
// Created by marko on 11. 11. 2024.
//

#ifndef OBJECT_H
#define OBJECT_H
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include "scene.h"

class Object {

public:
    glm::vec3 globalPosition() {
        return {0,0,0};
    };


    glm::vec3 position{0,0,0};
    // position - local and global
    // local position is noraml position
    // global position is parent position + local position
    // first parent will have a global position of 0 aka I guess if parent object is null local position is global position
    // but I also need a pointer to parent then

    glm::vec3 rotation{0,0,0};
    glm::vec3 scale{1,1,1};
    glm::mat4 modelMatrix{1};
    glm::vec3 color;

    std::vector<std::unique_ptr<Object>> children;
    std::unique_ptr<Object> parent;

    virtual bool update(Scene &scene, float dt) = 0;

    virtual void render(Scene &scene) = 0;

};

#endif //OBJECT_H

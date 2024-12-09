//
// Created by marko on 8. 12. 2024.
//

#ifndef INSTANCE_H
#define INSTANCE_H
#include "src/project/model.h"

class Instance : public Model {
public:
    Lamp(std::string const &path, bool gamma = false) : Model(path, gamma) {}
};
#endif //INSTANCE_H

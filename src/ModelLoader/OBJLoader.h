//
// Created by auser on 5/5/25.
//

#ifndef VULKAN_OBJLOADER_H
#define VULKAN_OBJLOADER_H

#include "AbsLoader.h"

class OBJLoader: public AbsLoader {
public:
    OBJLoader(std::string modelPath);

    bool load() override;
private:
};


#endif //VULKAN_OBJLOADER_H

//
// Created by auser on 5/6/25.
//

#ifndef VULKAN_ASSIMPLOADER_H
#define VULKAN_ASSIMPLOADER_H

#include "AbsLoader.h"

class AssimpLoader: public AbsLoader {
public:
    AssimpLoader(std::string modelPath);

    bool load() override;
private:
};


#endif //VULKAN_ASSIMPLOADER_H

//
// Created by auser on 5/5/25.
//

#ifndef VULKAN_MODELLOADER_H
#define VULKAN_MODELLOADER_H

#include "AbsLoader.h"

#include <memory>

namespace ModelLoader {

    std::unique_ptr<AbsLoader> createLoader(const std::string& modelpath);

}

#endif //VULKAN_MODELLOADER_H

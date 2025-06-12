//
// Created by auser on 5/5/25.
//

#include "ModelLoader.h"
#include "AssimpLoader.h"
#include "OBJLoader.h"

#include <filesystem>

namespace ModelLoader {

    namespace fs = std::filesystem;

    static std::unordered_map<std::string, std::function<std::unique_ptr<AbsLoader>(std::string)>> extToLoaderMap = {
            {".obj" , [](std::string path) { return std::make_unique<AssimpLoader>(std::move(path)); } },
            {".gltf", [](std::string path) { return std::make_unique<AssimpLoader>(std::move(path)); } },
            {".glb" , [](std::string path) { return std::make_unique<AssimpLoader>(std::move(path)); } },
            {".fbx" , [](std::string path) { return std::make_unique<AssimpLoader>(std::move(path)); } },
    };

    std::unique_ptr<AbsLoader> createLoader(const std::string& modelpath) {
        fs::path path(modelpath);
        std::string ext = path.extension().string();
        auto it = extToLoaderMap.find(ext);
        if (it != extToLoaderMap.end()) {
            return it->second(modelpath);
        } else {
            throw std::runtime_error("Unsupported format");
        }
    }

}
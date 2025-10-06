//
// Created by igor on 10/6/25.
//

#ifndef VULKAN_LOADER_HPP
#define VULKAN_LOADER_HPP

#include <string>
#include <vector>

#include "BBox.hpp"
#include "Mesh.hpp"
#include "Material.hpp"
#include "Vertex.hpp"

namespace crv::model {
    class AbsLoader;

    class Loader {
    public:
        Loader(std::string modelPath);
        ~Loader();

        bool load();

        [[nodiscard]] BBox bbox() const;
        [[nodiscard]] const std::vector<Mesh>& meshes() const;
        [[nodiscard]] const std::vector<Material>& materials() const;
        [[nodiscard]] const std::vector<Vertex>& vertices() const;
        [[nodiscard]] const std::vector<uint32_t>& indices() const;

        void clear();
    protected:
        static AbsLoader* getLoader(std::string path);

        AbsLoader* mLoader;
    };

}

#endif //VULKAN_LOADER_HPP

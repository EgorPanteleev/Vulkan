//
// Created by auser on 5/5/25.
//

#include "OBJLoader.h"
#include "MessageLogger.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>


OBJLoader::OBJLoader(std::string modelPath): AbsLoader(OBJ, std::move(modelPath)) {
}

bool OBJLoader::load() {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, mModelPath.c_str());

    if (!warn.empty()) WARNING << "Warning: " << warn;

    if (!err.empty()) ERROR << "Error: " << err;

    if (!ret) {
        ERROR << "Failed to load glTF: " << mModelPath;
        return false;
    }

    std::unordered_map<Vertex, uint32_t> uniqueVertices;

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            Vertex vertex{};
            vertex.pos = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
            };
            vertex.texCoord = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
            };
            vertex.color = {1.0f, 1.0f, 1.0f};

            if (uniqueVertices.count(vertex) == 0) {
                uniqueVertices[vertex] = static_cast<uint32_t>(mVertices.size());
                mVertices.push_back(vertex);
            }

            mIndices.push_back(uniqueVertices[vertex]);
        }
    }

    INFO << "Vertices size - " << mVertices.size();
    INFO << "Indices size - " << mIndices.size();
    MESSAGE << "Successfully loaded obj file: " << mModelPath;
    return ret;
}
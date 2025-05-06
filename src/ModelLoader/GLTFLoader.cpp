////
//// Created by auser on 5/5/25.
////
//
//#include "GLTFLoader.h"
//#include "MessageLogger.h"
//
//#define TINYGLTF_IMPLEMENTATION
//#define STB_IMAGE_WRITE_IMPLEMENTATION
//
//#include <filesystem>
//
//// If you use external json (optional)
//#define TINYGLTF_NO_EXTERNAL_IMAGE
//#include <tiny_gltf.h>
//
//#include <glm/glm.hpp>
//#include <glm/gtc/type_ptr.hpp>
//
//GLTFLoader::GLTFLoader(std::string modelPath): AbsLoader(GLTF, std::move(modelPath)) {}
//
//glm::mat4 getLocalTransform(const tinygltf::Node& node) {
//    glm::mat4 translation = glm::mat4(1.0f);
//    glm::mat4 rotation = glm::mat4(1.0f);
//    glm::mat4 scale = glm::mat4(1.0f);
//
//    if (!node.translation.empty()) {
//        translation = glm::translate(glm::mat4(1.0f),
//                                     glm::vec3(node.translation[0], node.translation[1], node.translation[2]));
//    }
//
//    if (!node.rotation.empty()) {
//        glm::quat q(node.rotation[3], node.rotation[0], node.rotation[1], node.rotation[2]);
//        rotation = glm::mat4_cast(q);
//    }
//
//    if (!node.scale.empty()) {
//        scale = glm::scale(glm::mat4(1.0f),
//                           glm::vec3(node.scale[0], node.scale[1], node.scale[2]));
//    }
//
//    if (!node.matrix.empty()) {
//        return glm::make_mat4(node.matrix.data());
//    }
//
//    return translation * rotation * scale;
//}
//
//// Modified: Process a single primitive and apply transform
//void processPrimitive(const tinygltf::Model& model, const tinygltf::Primitive& primitive,
//                      std::vector<Vertex>& vertices, std::vector<uint32_t>& indices,
//                      const glm::mat4& transform) {
//    // Indices
//    const auto& indicesAccessor = model.accessors[primitive.indices];
//    const auto& indicesView = model.bufferViews[indicesAccessor.bufferView];
//    const auto& indicesBuffer = model.buffers[indicesView.buffer];
//    const uint8_t* indicesData = indicesBuffer.data.data() + indicesView.byteOffset + indicesAccessor.byteOffset;
//    size_t indicesStride = indicesView.byteStride;
//    if (indicesStride == 0) {
//        indicesStride = tinygltf::GetComponentSizeInBytes(indicesAccessor.componentType) *
//                        tinygltf::GetNumComponentsInType(indicesAccessor.type);
//    }
//
//    std::vector<uint32_t> primitiveIndices;
//    for (size_t i = 0; i < indicesAccessor.count; ++i) {
//        const uint8_t* indexPtr = indicesData + i * indicesStride;
//        switch (indicesAccessor.componentType) {
//            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
//                primitiveIndices.push_back(*reinterpret_cast<const uint8_t*>(indexPtr));
//                break;
//            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
//                primitiveIndices.push_back(*reinterpret_cast<const uint16_t*>(indexPtr));
//                break;
//            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
//                primitiveIndices.push_back(*reinterpret_cast<const uint32_t*>(indexPtr));
//                break;
//            default:
//                throw std::runtime_error("Unsupported index component type");
//        }
//    }
//
//    // Vertex attributes
//    auto posIt = primitive.attributes.find("POSITION");
//    auto colorIt = primitive.attributes.find("COLOR_0");
//    auto texCoordIt = primitive.attributes.find("TEXCOORD_0");
//    if (posIt == primitive.attributes.end()) {
//        throw std::runtime_error("Missing required vertex attributes");
//    }
//
//    const auto& posAccessor = model.accessors[posIt->second];
//    const auto& colorAccessor = (colorIt != primitive.attributes.end())
//                                ? model.accessors[colorIt->second] : tinygltf::Accessor{};
//    const auto& texCoordAccessor = (texCoordIt != primitive.attributes.end())
//                                   ? model.accessors[texCoordIt->second] : tinygltf::Accessor{};
//
//    auto readData = [&](const tinygltf::Accessor& accessor) {
//        const auto& view = model.bufferViews[accessor.bufferView];
//        const auto& buffer = model.buffers[view.buffer];
//        const uint8_t* data = buffer.data.data() + view.byteOffset + accessor.byteOffset;
//        size_t stride = view.byteStride;
//        if (stride == 0) {
//            stride = tinygltf::GetComponentSizeInBytes(accessor.componentType) *
//                     tinygltf::GetNumComponentsInType(accessor.type);
//        }
//        return std::make_pair(data, stride);
//    };
//
//    const auto [posData, posStride] = readData(posAccessor);
//    const auto [colorData, colorStride] = colorIt != primitive.attributes.end()
//                                          ? readData(colorAccessor) : std::make_pair(nullptr, 0);
//    const auto [texData, texStride] = texCoordIt != primitive.attributes.end()
//                                      ? readData(texCoordAccessor) : std::make_pair(nullptr, 0);
//
//    std::vector<Vertex> primitiveVertices;
//    for (size_t i = 0; i < posAccessor.count; ++i) {
//        const float* pos = reinterpret_cast<const float*>(posData + i * posStride);
//
//        // Bake transformation into position
//        glm::vec4 transformedPos = transform * glm::vec4(pos[0], pos[1], pos[2], 1.0f);
//        glm::vec3 position = glm::vec3(transformedPos);
//
//        glm::vec3 color = glm::vec3(1.0f);
//        glm::vec2 texCoord = glm::vec2(0.0f);
//
//        if (colorData) {
//            const float* c = reinterpret_cast<const float*>(colorData + i * colorStride);
//            color = glm::vec3(c[0], c[1], c[2]);
//        }
//
//        if (texData) {
//            const float* t = reinterpret_cast<const float*>(texData + i * texStride);
//            texCoord = glm::vec2(t[0], t[1]);
//        }
//
//        primitiveVertices.emplace_back(position, color, texCoord);
//    }
//
//    // Adjust indices and combine
//    size_t vertexOffset = vertices.size();
//    vertices.insert(end(vertices), begin(primitiveVertices), end(primitiveVertices));
//    for (uint32_t index : primitiveIndices) {
//        indices.push_back(vertexOffset + index);
//    }
//}
//
//// Modified: Traverse scene nodes and apply transforms recursively
//void processNode(const tinygltf::Model& model, const tinygltf::Node& node,
//                 std::vector<Vertex>& vertices, std::vector<uint32_t>& indices,
//                 const glm::mat4& parentTransform) {
//    glm::mat4 localTransform = getLocalTransform(node);
//    glm::mat4 worldTransform = parentTransform * localTransform;
//
//    if (node.mesh >= 0) {
//        const auto& mesh = model.meshes[node.mesh];
//        for (const auto& primitive : mesh.primitives) {
//            processPrimitive(model, primitive, vertices, indices, worldTransform);
//        }
//    }
//
//    for (int child : node.children) {
//        processNode(model, model.nodes[child], vertices, indices, worldTransform);
//    }
//}
//
//// Modified: GLTFLoader class loading function
//bool GLTFLoader::load() {
//    tinygltf::Model model;
//    tinygltf::TinyGLTF loader;
//    std::string err, warn;
//
//    std::filesystem::path path(mModelPath);
//    if (path.extension() == ".glb") {
//        if (!loader.LoadBinaryFromFile(&model, &err, &warn, mModelPath)) {
//            throw std::runtime_error("Failed to load glTF: " + err);
//        }
//    } else {
//        if (!loader.LoadASCIIFromFile(&model, &err, &warn, mModelPath)) {
//            throw std::runtime_error("Failed to load glTF: " + err);
//        }
//    }
//    if (model.defaultScene >= 0) {
//        for (int node : model.scenes[model.defaultScene].nodes) {
//            processNode(model, model.nodes[node], mVertices, mIndices, glm::mat4(1.0f));
//        }
//    }
//
//
//    return true;
//}
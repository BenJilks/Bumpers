#pragma once

#include "gameobject/forward.hpp"
#include "engine/forward.hpp"
#include "mtl_loader.hpp"
#include <string>
#include <functional>
#include <optional>
#include <glm/glm.hpp>

namespace Engine::ColladaLoader
{
    
    struct ModelMetaData
    {
        std::string name;
        Material material;
        glm::vec3 translation { 0, 0, 0 };
        glm::vec3 scale { 1, 1, 1 };
        glm::vec3 rotation { 0, 0, 0 };
    };

    Object::GameObject *from_file(
        Object::GameObject &parent, const std::string &file_path,
        std::function<void(Object::GameObject&, Engine::MeshBuilder&, ModelMetaData)> on_object);
    
}


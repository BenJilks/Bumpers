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
        Material material;
        glm::mat4 transform;
    };

    Object::GameObject *from_file(
        Object::GameObject &parent, const std::string &file_path,
        std::function<void(Object::GameObject&, Engine::MeshBuilder&, ModelMetaData)> on_object);
    
}

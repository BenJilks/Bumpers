#pragma once

#include "gameobject/forward.hpp"
#include "engine/forward.hpp"
#include <string>
#include <functional>

namespace Engine::ObjLoader
{
    
    struct ModelMetaData
    {
        std::string matrial_name;
    };

    Object::GameObject *from_file(
        Object::GameObject &parent, const std::string &file_path,
        std::function<void(Object::GameObject&, Engine::MeshBuilder&, ModelMetaData)> on_object);
    
}

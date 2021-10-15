#pragma once

#include "engine/forward.hpp"
#include <string>
#include <memory>
#include <glm/glm.hpp>

namespace Engine
{

    struct Material
    {
        std::string name;
        glm::vec3 color { 1, 1, 1 };
        glm::vec3 specular_color { 1, 1, 1 };
        glm::vec3 emission_color { 0, 0, 0 };
        float normal_map_strength { 1 };
        float specular_focus { 32 };
        float metallic { 0 };
        std::shared_ptr<Texture> diffuse_map;
        std::shared_ptr<Texture> normal_map;
    };

}

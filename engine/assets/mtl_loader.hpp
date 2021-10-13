#pragma once

#include "engine/forward.hpp"
#include "engine/graphics/mesh/material.hpp"
#include <string>
#include <map>
#include <memory>
#include <glm/glm.hpp>

namespace Engine::MtlLoader
{

    std::map<std::string, Material> from_file(const std::string &file_path);

}

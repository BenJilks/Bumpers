/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "gameobject/forward.hpp"
#include "engine/forward.hpp"
#include "engine/graphics/mesh/material.hpp"
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

    Object::GameObject *open(
        Object::GameObject &parent, const AssetRepository&, std::string_view name,
        const std::function<void(Object::GameObject&, Engine::MeshBuilder&, ModelMetaData)> &on_object);
    
}


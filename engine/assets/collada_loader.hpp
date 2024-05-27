/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "engine/forward.hpp"
#include "engine/graphics/mesh/material.hpp"
#include "gameobject/forward.hpp"
#include <functional>
#include <glm/glm.hpp>
#include <string>

namespace Engine::ColladaLoader {

struct ModelMetaData {
    std::string name;
    Material material;
    glm::vec3 translation { 0, 0, 0 };
    glm::vec3 scale { 1, 1, 1 };
    glm::vec3 rotation { 0, 0, 0 };
};

Object::GameObject* open(
    Object::GameObject& parent, AssetRepository const&, std::string_view name,
    std::function<void(Object::GameObject&, Engine::MeshBuilder&, ModelMetaData)> const& on_object);

}

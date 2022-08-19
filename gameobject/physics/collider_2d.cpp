/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "collider_2d.hpp"
using namespace Engine;
using namespace Object;

Collider2D::Collider2D(std::shared_ptr<CollisionShape2D> shape)
    : m_shape(shape)
{
}


/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "gameobject.hpp"
using namespace Object;

GameObject& GameObject::add_child()
{
    auto child = std::unique_ptr<GameObject>(new GameObject);
    child->m_parent = this;

    m_children.push_back(std::move(child));
    return *m_children.back();
}

GameObject& GameObject::clone(GameObject& parent)
{
    auto object = std::unique_ptr<GameObject>(new GameObject);
    for (auto const& component : m_components) {
        object->m_components.push_back(std::move(component->clone()));
    }

    for (auto const& child : m_children) {
        child->clone(*object);
    }

    object->m_parent = &parent;
    parent.m_children.push_back(std::move(object));
    return *parent.m_children.back();
}

void GameObject::update(float delta)
{
    if (!m_enabled) {
        return;
    }

    for (auto& component : m_components) {
        component->update(*this, delta);
    }

    for (auto& child : m_children) {
        child->update(delta);
    }
}

void GameObject::step_physics(float by)
{
    if (!m_enabled) {
        return;
    }

    for (auto& component : m_components) {
        component->step_physics(*this, by);
    }

    for (auto& child : m_children) {
        child->step_physics(by);
    }
}

void GameObject::init()
{
    for (auto& component : m_components) {
        component->init(*this);
    }

    for (auto& child : m_children) {
        child->init();
    }
}

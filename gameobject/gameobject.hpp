/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "component.hpp"
#include <algorithm>
#include <memory>
#include <vector>

namespace Object {

enum class IteratorDecision {
    Continue,
    Break,
};

class GameObject {
public:
    GameObject& add_child();
    GameObject& clone(GameObject& parent);

    virtual ~GameObject() = default;

    template<typename T, typename... Args>
    T& add_component(Args&&... args)
    {
        m_components.push_back(std::move(T::construct(args...)));
        return static_cast<T&>(*m_components.back());
    }

    [[nodiscard]] inline GameObject const* parent() const { return m_parent; }
    [[nodiscard]] inline bool enabled() const { return m_enabled; }
    inline void set_enabled(bool value) { m_enabled = value; }

    void update(float delta);
    void step_physics(float by);
    void init();

    template<typename Func>
    IteratorDecision for_each(Func callback)
    {
        if (!m_enabled) {
            return IteratorDecision::Continue;
        }

        if (callback(*this) == IteratorDecision::Break) {
            return IteratorDecision::Break;
        }

        for (auto& child : m_children) {
            if (child->for_each(callback) == IteratorDecision::Break) {
                return IteratorDecision::Break;
            }
        }

        return IteratorDecision::Continue;
    }

    template<typename Func>
    void for_each_child(Func callback)
    {
        for (auto& child : m_children) {
            if (callback(*child) == IteratorDecision::Break) {
                break;
            }
        }
    }

    template<typename T>
    T const* first() const
    {
        // FIXME: This can be optimized with hash maps
        for (auto const& component : m_components) {
            if (component->is<T>()) {
                return static_cast<T const*>(&*component);
            }
        }

        return nullptr;
    }

    template<typename T>
    T* first()
    {
        return const_cast<T*>(const_cast<GameObject const*>(this)->first<T>());
    }

    template<typename T>
    std::vector<T const*> get() const
    {
        // FIXME: This can be optimized with hash maps
        std::vector<T const*> output;
        for (auto const& component : m_components) {
            if (component->is<T>()) {
                output.push_back(static_cast<T const*>(&*component));
            }
        }

        return output;
    }

    template<typename T>
    std::vector<T*> get()
    {
        // FIXME: This can be optimized with hash maps
        std::vector<T*> output;
        for (auto& component : m_components) {
            if (component->is<T>()) {
                output.push_back(static_cast<T*>(&*component));
            }
        }

        return output;
    }

protected:
    GameObject() = default;

private:
    GameObject const* m_parent { nullptr };
    std::vector<std::unique_ptr<GameObject>> m_children;
    std::vector<std::unique_ptr<Component>> m_components;

    bool m_enabled { true };
};

}

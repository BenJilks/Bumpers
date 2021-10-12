#pragma once

#include <memory>
#include <vector>
#include <algorithm>
#include "component.hpp"

namespace Object
{

    enum class ItoratorDecision
    {
        Continue,
        Break,
    };

    class GameObject
    {
    public:
        GameObject& add_child();
        GameObject& clone(GameObject &parent);
        
        virtual ~GameObject()
        {
        }

        template<typename T, typename... Args>
        T& add_component(Args&&... args)
        {
            m_components.push_back(std::move(T::construct(args...)));
            return static_cast<T&>(*m_components.back());
        }

        inline const GameObject* parent() const { return m_parent; }
        inline bool enabled() const { return m_enabled; }
        inline void set_enabled(bool value) { m_enabled = value; }

        void update(float delta);
        void step_physics(float by);
        void init();

        template<typename Func>
        ItoratorDecision for_each(Func callback) 
        {
            if (!m_enabled)
                return ItoratorDecision::Continue;

            if (callback(*this) == ItoratorDecision::Break)
                return ItoratorDecision::Break;

            for (auto& child : m_children)
            {
                if (child->for_each(callback) == ItoratorDecision::Break)
                    return ItoratorDecision::Break;
            }

            return ItoratorDecision::Continue;
        }

        template<typename Func>
        void for_each_child(Func callback)
        {
            for (auto& child : m_children)
            {
                if (callback(*child) == ItoratorDecision::Break)
                    break;
            }
        }

        template<typename T>
        const T* first() const
        {
            // FIXME: This can be optimized with hash maps
            for (const auto& component : m_components)
            {
                if (component->is<T>())
                    return static_cast<const T*>(&*component);
            }

            return nullptr;
        }

        template<typename T>
        T *first()
        {
            return const_cast<T*>(const_cast<const GameObject*>(this)->first<T>());
        }

        template<typename T>
        std::vector<const T*> get() const
        {
            // FIXME: This can be optimized with hash maps
            std::vector<const T*> output;
            for (const auto& component : m_components)
            {
                if (component->is<T>())
                    output.push_back(static_cast<const T*>(&*component));
            }

            return output;
        }

        template<typename T>
        std::vector<T*> get()
        {
            // FIXME: This can be optimized with hash maps
            std::vector<T*> output;
            for (auto& component : m_components)
            {
                if (component->is<T>())
                    output.push_back(static_cast<T*>(&*component));
            }

            return output;
        }

    protected:
        GameObject() = default;

    private:
        const GameObject* m_parent { nullptr };
        std::vector<std::unique_ptr<GameObject>> m_children;
        std::vector<std::unique_ptr<Component>> m_components;

        bool m_enabled { true };

    };

}

/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <memory>
#include <typeinfo>
#include "forward.hpp"

namespace Object
{

    class Component
    {
        friend GameObject;

    public:
        virtual ~Component()
        {
        }
        
        template<typename T>
        bool is()
        {
            return type_id() == typeid(T).name();
        }

        virtual const char *type_id() const = 0;
        virtual void init(GameObject&) {}
        virtual void update(GameObject&, float delta) {}
        virtual void step_physics(GameObject&, float by) {}

    private:
        virtual std::unique_ptr<Component> clone() = 0;

    };

    template<typename T>
    class ComponentBase : public Component
    {
        friend GameObject;

    public:
        virtual const char* type_id() const final
        {
            return typeid(T).name();
        }

    private:
        template<typename... Args>
        static std::unique_ptr<T> construct(Args&&... args)
        {
            return std::unique_ptr<T>(new T(args...));
        }

        virtual std::unique_ptr<Component> clone() final
        {
            return std::unique_ptr<T>(new T(static_cast<const T&>(*this)));
        }

    };

}

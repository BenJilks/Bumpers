#pragma once

#include "component.hpp"

namespace Object
{

    class Camera : public ComponentBase<Camera>
    {
        friend ComponentBase<Camera>;

    public:

    private:
        Camera() = default;
    };

}

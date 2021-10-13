#pragma once

#include <array>

namespace Engine::ObjLikeUtils
{

    template<unsigned int count>
    std::array<float, count> float_args(const char *arg_str)
    {
        static_assert(count <= 3);
        char *arg_ptr = const_cast<char*>(arg_str);

        std::array<float, count> argsf;
        for (int i = 0; i < count; i++)
            argsf[i] = strtod(arg_ptr, &arg_ptr);
        return argsf;
    }

}

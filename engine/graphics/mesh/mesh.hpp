#pragma once

#include <memory>
#include <vector>
#include <array>
#include <glm/glm.hpp>
#include <cstdint>
#include "engine/forward.hpp"

typedef unsigned int GLuint;
typedef unsigned int GLenum;

namespace Engine
{

    class Mesh
    {
    public:
        static std::shared_ptr<Mesh> construct(const MeshBuilder &builder);
        ~Mesh();

        void draw() const;

    private:
        Mesh() = default;

        template<int size, GLuint gl_type, typename T>
        void bind_buffer_data(int index, std::vector<T> data);
        void bind_indicies(int index, std::vector<uint32_t>);
        
        GLuint m_vao;
        std::array<GLuint, 3> m_vbo;
        int m_count { 0 };
        int m_instance_count { 0 };

    };

}

#include "obj_loader.hpp"
#include "graphics/mesh/mesh_builder.hpp"
#include "gameobject/gameobject.hpp"
#include "gameobject/transform.hpp"
#include "gameobject/mesh_render.hpp"
#include <iostream>
#include <fstream>
#include <cstring>
#include <cassert>
#include <string_view>
#include <glm/glm.hpp>
using namespace Engine;
using namespace Object;
using namespace glm;

enum class LineType
{
    Unkown,
    UseMtl,
    Object,
    Vertex,
    TextureCoord,
    Face,
};

struct Line
{
    LineType type;
    int component_count;
    union
    {
        float argsf[3];
        std::array<unsigned, 2> argsi[3];
        char name[80];
    };
};

static Line name_arg(const char *arg_str, LineType type)
{
    Line line { .type = type };
    strcpy(line.name, arg_str);

    return line;
}

template<unsigned int count>
static Line float_args(const char *arg_str, LineType type)
{
    static_assert(count <= 3);
    char *arg_ptr = const_cast<char*>(arg_str);

    Line line { .type = type };
    for (int i = 0; i < count; i++)
        line.argsf[i] = strtod(arg_ptr, &arg_ptr);
    
    return line;
}

template<unsigned int count>
static Line int_args(const char *arg_str, LineType type)
{
    static_assert(count <= 3);
    char *arg_ptr = const_cast<char*>(arg_str);

    Line line { .type = type };
    for (int i = 0; i < count; i++)
    {
        int component_index = 0;
        for (;;)
        {
            assert(component_index < line.argsi[i].size());

            line.argsi[i][component_index++] = strtoul(arg_ptr, &arg_ptr, 10);
            if (*arg_ptr != '/')
                break;
            arg_ptr += 1;
        }

        line.component_count = component_index;
    }
    
    return line;
}

static Line parse_line(const std::string &line)
{
    auto type_end = strchr(line.c_str(), ' ');
    auto type_name = std::string_view(line.c_str(), type_end - line.c_str());
    if (type_name == "usemtl")
        return name_arg(type_end + 1, LineType::UseMtl);
    if (type_name == "o")
        return name_arg(type_end + 1, LineType::Object);
    if (type_name == "v")
        return float_args<3>(type_end + 1, LineType::Vertex);
    if (type_name == "vt")
        return float_args<2>(type_end + 1, LineType::TextureCoord);
    if (type_name == "f")
        return int_args<3>(type_end + 1, LineType::Face);
    
    return Line { .type = LineType::Unkown };
}

struct ModelState
{
    ObjLoader::ModelMetaData current_meta_data;
    MeshBuilder current_mesh;
    std::vector<vec3> verticies;
    std::vector<vec2> texture_coords;
    unsigned int index_count { 0 };
};

static void parse_line(GameObject &parent, const std::string &line_str,
    ModelState &state, std::function<void(GameObject&, MeshBuilder&, ObjLoader::ModelMetaData)> on_object)
{
    auto line = parse_line(line_str);
    switch (line.type)
    {
        case LineType::UseMtl:
            state.current_meta_data.matrial_name = line.name;
            break;
        case LineType::Vertex:
            state.verticies.push_back(vec3(line.argsf[0], -line.argsf[1], line.argsf[2]));
            break;
        case LineType::TextureCoord:
            state.texture_coords.push_back(vec2(line.argsf[0], line.argsf[1]));
            break;
        case LineType::Face:
            for (int i = 2; i >= 0; i--)
            {
                if (line.component_count > 0)
                    state.current_mesh.add_vertex(state.verticies[line.argsi[i][0] - 1]);
                if (line.component_count > 1)
                    state.current_mesh.add_texture_coord(state.texture_coords[line.argsi[i][1] - 1]);
            }

            state.current_mesh.add_indicies({ state.index_count, state.index_count + 1, state.index_count + 2 });
            state.index_count += 3;
            break;

        case LineType::Object:
            if (!state.current_mesh.is_empty())
            {
                on_object(parent.add_child(), state.current_mesh, state.current_meta_data);
                state.index_count = 0;
                state.current_mesh = {};
                state.current_meta_data = {};
            }
            break;
            
        default:
            break;
    }
}

GameObject *ObjLoader::from_file(
    GameObject &parent, const std::string &file_path,
    std::function<void(GameObject&, MeshBuilder&, ModelMetaData)> on_object)
{
    std::ifstream obj_stream(file_path.c_str());
    std::string line_str;
    
    ModelState state;
    while (std::getline(obj_stream, line_str))
        parse_line(parent, line_str, state, on_object);
    
    if (!state.current_mesh.is_empty())
        on_object(parent.add_child(), state.current_mesh, state.current_meta_data);
    return nullptr;
}

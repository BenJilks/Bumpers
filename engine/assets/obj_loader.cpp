#include "obj_loader.hpp"
#include "obj_like_utils.hpp"
#include "engine/graphics/mesh/mesh_builder.hpp"
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
    MtlLib,
    UseMtl,
    Object,
    Vertex,
    Normal,
    TextureCoord,
    Face,
};

struct Line
{
    LineType type;

    int component_count;
    std::array<float, 3> argsf;
    std::array<std::array<unsigned, 3>, 3> argsi;
    std::string name;
};

static Line name_arg(const char *arg_str, LineType type)
{
    Line line { .type = type };
    line.name = arg_str;
    return line;
}

template<unsigned int count>
static Line float_args(const char *arg_str, LineType type)
{
    Line line;
    line.type = type;
    memcpy(line.argsf.data(), ObjLikeUtils::float_args<count>(arg_str).data(), sizeof(float) * count);
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
    auto type_end = std::strchr(line.c_str(), ' ');
    auto type_name = std::string_view(line.c_str(), type_end - line.c_str());
    if (type_name == "mtllib")
        return name_arg(type_end + 1, LineType::MtlLib);
    if (type_name == "usemtl")
        return name_arg(type_end + 1, LineType::UseMtl);
    if (type_name == "o")
        return name_arg(type_end + 1, LineType::Object);
    if (type_name == "v")
        return float_args<3>(type_end + 1, LineType::Vertex);
    if (type_name == "vn")
        return float_args<3>(type_end + 1, LineType::Normal);
    if (type_name == "vt")
        return float_args<2>(type_end + 1, LineType::TextureCoord);
    if (type_name == "f")
        return int_args<3>(type_end + 1, LineType::Face);
    
    return Line { .type = LineType::Unkown };
}

struct ModelState
{
    std::string relative_path;
    std::map<std::string, Material> material_lib;
    
    ObjLoader::ModelMetaData current_meta_data;
    MeshBuilder current_mesh;
    std::vector<vec3> verticies;
    std::vector<vec3> normals;
    std::vector<vec2> texture_coords;
    unsigned int index_count { 0 };
};

static void compute_tangents(ModelState &state, const Line &line)
{
    auto pos0 = state.verticies[line.argsi[0][0] - 1];
    auto pos1 = state.verticies[line.argsi[1][0] - 1];
    auto pos2 = state.verticies[line.argsi[2][0] - 1];
    auto uv0 = state.texture_coords[line.argsi[0][1] - 1];
    auto uv1 = state.texture_coords[line.argsi[1][1] - 1];
    auto uv2 = state.texture_coords[line.argsi[2][1] - 1];

    auto edge0 = pos1 - pos0;
    auto edge1 = pos2 - pos0;
    auto delta_uv0 = uv1 - uv0;
    auto delta_uv1 = uv2 - uv0;
    float f = 1.0f / (delta_uv0.x * delta_uv1.y - delta_uv1.x * delta_uv0.y);

    auto tangent = (edge0 * delta_uv1.y - edge1 * delta_uv0.y) * f;
    for (int i = 0; i < 3; i++)
        state.current_mesh.add_tangent(tangent);

    auto bitangent = (edge1 * delta_uv0.x - edge0 * delta_uv1.x) * f;
    for (int i = 0; i < 3; i++)
        state.current_mesh.add_bitangent(bitangent);
}

static void parse_line(GameObject &parent, const std::string &line_str,
    ModelState &state, std::function<void(GameObject&, MeshBuilder&, ObjLoader::ModelMetaData)> on_object)
{
    auto line = parse_line(line_str);
    switch (line.type)
    {
        case LineType::MtlLib:
            state.material_lib = MtlLoader::from_file(state.relative_path + "/" + line.name);
            break;
        case LineType::UseMtl:
            if (state.material_lib.contains(line.name))
                state.current_meta_data.material = state.material_lib[line.name];
            break;
        case LineType::Vertex:
            state.verticies.push_back(vec3(line.argsf[0], line.argsf[1], line.argsf[2]));
            break;
        case LineType::Normal:
            state.normals.push_back(vec3(line.argsf[0], line.argsf[1], line.argsf[2]));
            break;
        case LineType::TextureCoord:
            state.texture_coords.push_back(vec2(line.argsf[0], line.argsf[1]));
            break;
        case LineType::Face:
            for (int i = 0; i < 3; i++)
            {
                if (line.component_count > 0)
                    state.current_mesh.add_vertex(state.verticies[line.argsi[i][0] - 1]);
                if (line.component_count > 1 && line.argsi[i][1] > 0)
                    state.current_mesh.add_texture_coord(state.texture_coords[line.argsi[i][1] - 1]);
                if (line.component_count > 2)
                    state.current_mesh.add_normal(state.normals[line.argsi[i][2] - 1]);
                
            }

            if (line.component_count > 2)
                compute_tangents(state, line);

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
        
        case LineType::Unkown:
            break;
    }
}

static std::string get_relative_path(const std::string &file_path)
{
    int end_pos = file_path.size() - 1;
    for (int i = end_pos; i >= 0; i--)
    {
        if (file_path[i] == '/')
        {
            end_pos = i;
            break;
        }
    }

    return file_path.substr(0, end_pos);
}

GameObject *ObjLoader::from_file(
    GameObject &parent, const std::string &file_path,
    std::function<void(GameObject&, MeshBuilder&, ModelMetaData)> on_object)
{
    std::ifstream obj_stream(file_path.c_str());
    std::string line_str;
    
    GameObject &model_object = parent.add_child();

    ModelState state;
    state.relative_path = get_relative_path(file_path);

    while (std::getline(obj_stream, line_str))
        parse_line(model_object, line_str, state, on_object);
    
    if (!state.current_mesh.is_empty())
        on_object(model_object.add_child(), state.current_mesh, state.current_meta_data);
    return &model_object;
}

#include "mtl_loader.hpp"
#include "obj_like_utils.hpp"
#include "engine/graphics/texture/image_texture.hpp"
#include "game/config.hpp"
#include <fstream>
#include <cstring>
#include <optional>
#include <iostream>
#include <glm/glm.hpp>
using namespace Engine;
using namespace glm;

enum class LineType
{
    Unkown,
    NewMtl,
    MapKd,
    MapBump,
    Kd,
    Ks,
    Ke,
    Ns,
};

struct Line
{
    LineType type;

    std::array<float, 3> argsf;
    std::string name;
    std::map<std::string, std::string> arguments;
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

static Line map_args(const char *arg_str, LineType type)
{
    enum State
    {
        Default,
        ArgumentName,
        ArgumentWhiteSpace,
        Argument,
        Path,
    };

    Line line { .type = type };
    State state = State::Default;

    std::map<std::string, std::string> arguments;
    std::string argument_name;
    std::string argument;
    std::string path;
    for (const char *c = arg_str; *c; c++)
    {
        switch (state)
        {
            case State::Default:
                if (!std::isspace(*c))
                {
                    if (*c == '-')
                        state = State::ArgumentName;
                    else
                        state = State::Path;
                }
                break;
            
            case State::ArgumentName:
                if (std::isspace(*c))
                    state = State::ArgumentWhiteSpace;
                else
                    argument_name += *c;
                break;
            
            case State::ArgumentWhiteSpace:
                if (!std::isspace(*c))
                {
                    argument += *c;
                    state = State::Argument;
                }
                break;
            
            case State::Argument:
                if (std::isspace(*c))
                {
                    state = State::Default;
                    arguments.insert(std::make_pair(argument_name, argument));
                    argument_name.clear();
                    argument.clear();
                }
                else
                {
                    argument += *c;
                }
                break;
            
            case State::Path:
                path += *c;
                break;
        }
    }

    line.name = path;
    line.arguments = arguments;
    return line;
}

static Line parse_line(const std::string &line)
{
    auto type_end = std::strchr(line.c_str(), ' ');
    auto type_name = std::string_view(line.c_str(), type_end - line.c_str());
    if (type_name == "newmtl")
        return name_arg(type_end + 1, LineType::NewMtl);
    if (type_name == "map_Kd")
        return name_arg(type_end + 1, LineType::MapKd);
    if (type_name == "map_Bump")
        return map_args(type_end + 1, LineType::MapBump);
    if (type_name == "Kd")
        return float_args<3>(type_end + 1, LineType::Kd);
    if (type_name == "Ks")
        return float_args<3>(type_end + 1, LineType::Ks);
    if (type_name == "Ke")
        return float_args<3>(type_end + 1, LineType::Ke);
    if (type_name == "Ns")
        return float_args<1>(type_end + 1, LineType::Ns);
    
    return Line { .type = LineType::Unkown };
}

std::map<std::string, Material> MtlLoader::from_file(const std::string &file_path)
{
    std::ifstream mtl_stream(file_path.c_str());
    std::string line_str;

    std::map<std::string, Material> lib;
    std::optional<Material> current_material;
    while (std::getline(mtl_stream, line_str))
    {
        auto line = parse_line(line_str);
        switch (line.type)
        {
            case LineType::NewMtl:
                if (current_material)
                    lib.insert(std::make_pair(current_material->name, *current_material));
                current_material = Material {};
                current_material->name = line.name;
                break;
            
            case LineType::MapKd:
                current_material->diffuse_map = ImageTexture::construct(ASSETS + "/" + line.name);
                break;

            case LineType::MapBump:
                current_material->normal_map = ImageTexture::construct(ASSETS + "/" + line.name);
                if (line.arguments.contains("bm"))
                    current_material->normal_map_strength = std::atof(line.arguments["bm"].c_str());
                break;

            case LineType::Kd:
                current_material->color = vec3(line.argsf[0], line.argsf[1], line.argsf[2]);
                break;

            case LineType::Ks:
                current_material->specular_color = vec3(line.argsf[0], line.argsf[1], line.argsf[2]);
                break;

            case LineType::Ke:
                current_material->emission_color = vec3(line.argsf[0], line.argsf[1], line.argsf[2]);
                break;

            case LineType::Ns:
                current_material->specular_focus = line.argsf[0];
                break;
            
            case LineType::Unkown:
                break;
        }
    }

    if (current_material)
        lib.insert(std::make_pair(current_material->name, *current_material));

    return lib;
}

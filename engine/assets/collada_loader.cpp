/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "collada_loader.hpp"
#include "asset_repository.hpp"
#include "engine/graphics/mesh/mesh_builder.hpp"
#include "engine/graphics/texture/image_texture.hpp"
#include "gameobject/gameobject.hpp"
#include <pugixml.hpp>
#include <iostream>
#include <memory>
#include <map>
#include <glm/glm.hpp>
using namespace Engine;
using namespace Object;
using namespace glm;

static std::string parse_id_selector(const pugi::xml_attribute &selector)
{
    return selector.as_string() + 1;
}

struct Source
{
    std::string id;
    std::vector<float> data;
    int stride;
};

static std::vector<float> load_float_array(const pugi::xml_node &float_array_node)
{
    std::vector<float> float_array;

    const char *raw_data = float_array_node.text().as_string();
    char *end;
    while (*raw_data != '\0')
    {
        float_array.push_back(std::strtof(raw_data, &end));
        raw_data = end;
    }

    return float_array;
}

static std::vector<uint32_t> load_int_array(const pugi::xml_node &int_array_node)
{
    std::vector<uint32_t> int_array;

    const char *raw_data = int_array_node.text().as_string();
    char *end;
    while (*raw_data != '\0')
    {
        int_array.push_back(std::strtol(raw_data, &end, 10));
        raw_data = end;
    }

    return int_array;
}

static Source load_source(const pugi::xml_node &source_node)
{
    Source source;
    source.id = source_node.attribute("id").value();
    source.data = load_float_array(source_node.child("float_array"));

    source.stride = source_node
        .child("technique_common")
        .child("accessor")
        .attribute("stride")
        .as_int();

    return source;
}

static std::map<std::string, Source> load_sources(const pugi::xml_node &mesh_node)
{
    std::map<std::string, Source> sources;
    for (const auto &source_node : mesh_node.children("source"))
    {
        auto source = load_source(source_node);
        sources[source.id] = source;
    }

    const auto &vertices_node = mesh_node.child("vertices");
    if (vertices_node)
    {
        const auto &vertices_input_node = vertices_node.child("input");
        const auto &vertices_id = vertices_node.attribute("id").as_string();
        sources[vertices_id] = sources[parse_id_selector(vertices_input_node.attribute("source"))];
    }

    return sources;
}

template<unsigned int count>
static vec<count, float> vec_at_index(
    uint32_t index, const std::vector<float> &points)
{
    vec<count, float> result_vec;
    for (int i = 0; i < count; i++)
        result_vec[i] = points[index * count + i];

    return result_vec;
};

static void compute_tangents(
    MeshBuilder &builder,
    const std::vector<float> &verticies,
    const std::vector<float> &texture_coords,
    const std::vector<uint32_t> &indicies,
    int index)
{
    auto pos0 = vec_at_index<3>(indicies[index + 3*0], verticies);
    auto pos1 = vec_at_index<3>(indicies[index + 3*1], verticies);
    auto pos2 = vec_at_index<3>(indicies[index + 3*2], verticies);
    auto uv0 = vec_at_index<2>(indicies[index + 3*0 + 2], texture_coords);
    auto uv1 = vec_at_index<2>(indicies[index + 3*1 + 2], texture_coords);
    auto uv2 = vec_at_index<2>(indicies[index + 3*2 + 2], texture_coords);

    auto edge0 = pos1 - pos0;
    auto edge1 = pos2 - pos0;
    auto delta_uv0 = uv1 - uv0;
    auto delta_uv1 = uv2 - uv0;
    float f = 1.0f / (delta_uv0.x * delta_uv1.y - delta_uv1.x * delta_uv0.y);

    auto tangent = (edge0 * delta_uv1.y - edge1 * delta_uv0.y) * f;
    for (int i = 0; i < 3; i++)
        builder.add_tangent(tangent);

    auto bitangent = (edge1 * delta_uv0.x - edge0 * delta_uv1.x) * f;
    for (int i = 0; i < 3; i++)
        builder.add_bitangent(bitangent);
}

static MeshBuilder load_mesh(const pugi::xml_node &mesh_node)
{
    MeshBuilder builder;
    auto sources = load_sources(mesh_node);

    const std::vector<float> *verticies = nullptr;
    const std::vector<float> *normals = nullptr;
    const std::vector<float> *texture_coords_0 = nullptr;
    const std::vector<float> *texture_coords_1 = nullptr;

    const auto &triangles_node = mesh_node.child("triangles");
    for (const auto &input_node : triangles_node.children("input"))
    {
        std::string semantic = input_node.attribute("semantic").as_string();
        int set = input_node.attribute("set").as_int();
        const auto &source = sources[parse_id_selector(input_node.attribute("source"))];
        if (semantic == "VERTEX")
            verticies = &source.data;
        else if (semantic == "NORMAL")
            normals = &source.data;
        else if (semantic == "TEXCOORD" && set == 0)
            texture_coords_0 = &source.data;
        else if (semantic == "TEXCOORD" && set == 1)
            texture_coords_1 = &source.data;
    }

    auto indicies = load_int_array(triangles_node.child("p"));
    uint32_t index_count = 0;
    for (int i = 0; i < indicies.size(); i += 3 * 3)
    {
        for (int j = 0; j < 3; j++)
        {
            auto index = i + j*3;
            builder.add_vertex(vec_at_index<3>(indicies[index + 0], *verticies));
            builder.add_normal(vec_at_index<3>(indicies[index + 1], *normals));

            auto uv0 = vec_at_index<2>(indicies[index + 2], *texture_coords_0);
            if (texture_coords_1)
                builder.add_uv01(uv0, vec_at_index<2>(indicies[index + 2], *texture_coords_1));
            else
                builder.add_uv0(uv0);
        }

        compute_tangents(builder,
            *verticies, *texture_coords_0, indicies, i);

        builder.add_indicies({ index_count, index_count + 1, index_count + 2 });
        index_count += 3;
    }

    return builder;
}

static std::map<std::string, MeshBuilder> load_meshes(
    const pugi::xml_node &meshes_node)
{
    std::map<std::string, MeshBuilder> meshes;
    for (const auto &geometry_node : meshes_node.children("geometry"))
    {
        const auto &id = geometry_node.attribute("id").as_string();
        auto mesh = load_mesh(geometry_node.child("mesh"));
        meshes[id] = mesh;
    }

    return meshes;
}

static std::map<std::string, std::shared_ptr<Texture>> load_images(
    const AssetRepository &assets,
    const pugi::xml_node &images_node)
{
    std::map<std::string, std::shared_ptr<Texture>> images;
    for (const auto &image_node : images_node.children("image"))
    {
        auto id = image_node.attribute("id").as_string();
        auto file_path = image_node.child("init_from").text().as_string();
        auto texture = ImageTexture::construct(assets, file_path);
        images[id] = texture;
    }

    return images;
}

static std::map<std::string, std::shared_ptr<Texture>> load_textures(
    const std::map<std::string, std::shared_ptr<Texture>> &image_library,
    const pugi::xml_node &profile_node)
{
    std::map<std::string, std::shared_ptr<Texture>> textures;
    for (const auto &param_node : profile_node.children("newparam"))
    {
        const auto &sid = param_node.attribute("sid").as_string();
        if (param_node.child("surface"))
        {
            const auto &image_id = param_node
                .child("surface")
                .child("init_from")
                .text()
                .as_string();
            
            if (!image_library.contains(image_id))
            {
                std::cerr << "Warning: Image '" << image_id << "', not found\n";
                continue;
            }
            textures[sid] = image_library.at(image_id);
        }
        else if (param_node.child("sampler2D"))
        {
            const auto &texture_id = param_node
                .child("sampler2D")
                .child("source")
                .text()
                .as_string();

            if (!textures.contains(texture_id))
            {
                std::cerr << "Warning: Texture '" << texture_id << "', not found\n";
                continue;
            }
            textures[sid] = textures[texture_id];
        }
    }

    return textures;
}

static vec3 load_color(const pugi::xml_node &color_node)
{
    auto float_array = load_float_array(color_node);
    return { float_array[0], float_array[1], float_array[2] };
}

static void load_lambert(
    const AssetRepository &assets,
    Material &material,
    std::map<std::string, std::shared_ptr<Texture>> texture_library,
    const pugi::xml_node &lambert_node)
{
    const auto &diffuse_node = lambert_node.child("diffuse");
    if (diffuse_node.child("texture"))
    {
        auto texture_id = diffuse_node
            .child("texture")
            .attribute("texture")
            .as_string();
        
        if (!texture_library.contains(texture_id))
            std::cerr << "Warning: Texture '" << texture_id << "' not found\n";
        else
            material.diffuse_map = texture_library[texture_id];
    }
    else if (diffuse_node.child("color"))
    {
        // FIXME: Extremely hacky
        material.diffuse_map = ImageTexture::construct(assets, "/textures/bumper/white.jpg");
        material.color = load_color(diffuse_node
            .child("color"));
    }
    
    if (lambert_node.child("emission"))
    {
        material.emission_color = load_color(lambert_node
            .child("emission")
            .child("color"));
    }

    if (lambert_node.child("specular"))
    {
        const auto &specular_node = lambert_node.child("specular");
        material.specular_color = load_color(specular_node.child("color"));
        
        if (specular_node.child("float"))
        {
            material.specular_focus = specular_node
                .child("float")
                .text()
                .as_float();
        }
    }

    if (lambert_node.child("reflectivity"))
    {
        material.specular_color = vec3(1.0) * lambert_node
            .child("reflectivity")
            .child("float")
            .text()
            .as_float();
    }

    if (lambert_node.child("metallic"))
    {
        material.metallic = lambert_node
            .child("metallic")
            .child("float")
            .text()
            .as_float();
    }
}

static void load_extra(
    Material &material,
    std::map<std::string, std::shared_ptr<Texture>> texture_library,
    const pugi::xml_node &extra_node)
{
    const auto &technique_node = extra_node.child("technique");

    if (technique_node.child("bump"))
    {
        const auto &bump = technique_node.child("bump");
        const auto &texture_id = bump
            .child("texture")
            .attribute("texture")
            .as_string();
        
        if (!texture_library.contains(texture_id))
            std::cerr << "Warning: Texture '" << texture_id << "' not found\n";
        else
            material.normal_map = texture_library[texture_id];

        if (bump.child("float"))
        {
            material.normal_map_strength = bump
                .child("float")
                .text()
                .as_float();
        }
    }

    if (technique_node.child("lightmap"))
    {
        const auto &texture_id = technique_node
            .child("lightmap")
            .child("texture")
            .attribute("texture")
            .as_string();

        if (!texture_library.contains(texture_id))
            std::cerr << "Warning: Texture '" << texture_id << "' not found\n";
        else
            material.light_map = texture_library[texture_id];
    }
}

static std::map<std::string, Material> load_effects(
    const AssetRepository &assets,
    const std::map<std::string, std::shared_ptr<Texture>> &image_library,
    const pugi::xml_node &effects_node)
{
    std::map<std::string, Material> effects;
    for (const auto &effect_node : effects_node.children("effect"))
    {
        auto id = effect_node.attribute("id").as_string();
        const auto &profile_node = effect_node.child("profile_COMMON");
        auto texture_library = load_textures(image_library, profile_node);
        
        Material material;
        const auto &technique_node = profile_node.child("technique");

        if (technique_node.child("lambert"))
        {
            load_lambert(assets, material, texture_library,
                technique_node.child("lambert"));
        }

        if (technique_node.child("extra"))
        {
            load_extra(material, texture_library, 
                technique_node.child("extra"));
        }
        
        effects[id] = material;
    }

    return effects;
}

static std::map<std::string, Material> load_materials(
    const std::map<std::string, Material> &effect_library,
    const pugi::xml_node &materials_node)
{
    std::map<std::string, Material> materials;
    for (const auto &material_node : materials_node.children("material"))
    {
        auto id = material_node.attribute("id").as_string();
        auto effect_id = material_node
            .child("instance_effect")
            .attribute("url");
        
        const auto &effect = effect_library.at(parse_id_selector(effect_id));
        materials[id] = effect;
    }
    
    return materials;
}

static mat4 load_matrix(const pugi::xml_node &matrix_node)
{
    mat4 matrix;
    auto float_array = load_float_array(matrix_node);
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
            matrix[i][j] = float_array[j*4 + i];
    }

    return matrix;
}

static vec3 load_vec3(const pugi::xml_node &vec3_node)
{
    auto float_array = load_float_array(vec3_node);
    return vec3(float_array[0], float_array[1], float_array[2]);
}

static vec4 load_vec4(const pugi::xml_node &vec4_node)
{
    auto float_array = load_float_array(vec4_node);
    return vec4(float_array[0], float_array[1], float_array[2], float_array[3]);
}

static vec3 load_rotation(const pugi::xml_node &node_node)
{
    vec3 rotation { 0, 0, 0 };
    for (const auto &rotate_node : node_node.children("rotate"))
    {
        auto data = load_vec4(rotate_node);
        rotation += vec3(data.x, data.y, data.z) * glm::radians(data.w);
    }

    return rotation;
}

GameObject *ColladaLoader::open(
    GameObject &parent, const AssetRepository &assets, std::string_view model_name,
    const std::function<void(GameObject&, Engine::MeshBuilder&, ModelMetaData)> &on_object)
{
    pugi::xml_document doc;
    
    auto result = doc.load(*assets.open(model_name));
    if (!result)
    {
        std::cerr << "Error: Parsing Collada file: " << result.description() << "\n";
        return nullptr;
    }

    const auto &root = doc.child("COLLADA");
    auto image_library = load_images(assets, root.child("library_images"));
    auto effect_library = load_effects(assets, image_library, root.child("library_effects"));
    auto material_library = load_materials(effect_library, root.child("library_materials"));
    auto mesh_library = load_meshes(root.child("library_geometries"));

    auto &model_object = parent.add_child();
    const auto &visual_scenes_node = root.child("library_visual_scenes");
    for (const auto &visual_scene_node : visual_scenes_node.children("visual_scene"))
    {
        for (const auto &node_node : visual_scene_node.children("node"))
        {
            const auto name = node_node.attribute("name").value();

            const auto &instance_geometry_node = node_node.child("instance_geometry");
            const auto &mesh_id = instance_geometry_node.attribute("url");
            const auto &material_id = instance_geometry_node
                .child("bind_material")
                .child("technique_common")
                .child("instance_material")
                .attribute("target");
            
            auto &mesh = mesh_library[parse_id_selector(mesh_id)];
            auto &material = material_library[parse_id_selector(material_id)];
            auto &gameobject = model_object.add_child();
            on_object(gameobject, mesh, ModelMetaData
            {
                .name = name,
                .material = material,
                .translation = load_vec3(node_node.child("translate")),
                .scale = load_vec3(node_node.child("scale")),
                .rotation = load_rotation(node_node),
            });
        }
    }

    return &model_object;
}


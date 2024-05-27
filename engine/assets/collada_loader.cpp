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
#include <glm/glm.hpp>
#include <iostream>
#include <map>
#include <memory>
#include <pugixml.hpp>
using namespace Engine;
using namespace Object;

static std::string parse_id_selector(pugi::xml_attribute const& selector)
{
    return selector.as_string() + 1;
}

struct Source {
    std::string id;
    std::vector<float> data;
    int stride { 0 };
};

static std::vector<float> load_float_array(pugi::xml_node const& float_array_node)
{
    std::vector<float> float_array;

    char const* raw_data = float_array_node.text().as_string();
    char* end;
    while (*raw_data != '\0') {
        float_array.push_back(std::strtof(raw_data, &end));
        raw_data = end;
    }

    return float_array;
}

static std::vector<uint32_t> load_int_array(pugi::xml_node const& int_array_node)
{
    std::vector<uint32_t> int_array;

    char const* raw_data = int_array_node.text().as_string();
    char* end;
    while (*raw_data != '\0') {
        int_array.push_back(std::strtol(raw_data, &end, 10));
        raw_data = end;
    }

    return int_array;
}

static Source load_source(pugi::xml_node const& source_node)
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

static std::map<std::string, Source> load_sources(pugi::xml_node const& mesh_node)
{
    std::map<std::string, Source> sources;
    for (auto const& source_node : mesh_node.children("source")) {
        auto source = load_source(source_node);
        sources[source.id] = source;
    }

    auto const& vertices_node = mesh_node.child("vertices");
    if (vertices_node) {
        auto const& vertices_input_node = vertices_node.child("input");
        auto const& vertices_id = vertices_node.attribute("id").as_string();
        sources[vertices_id] = sources[parse_id_selector(vertices_input_node.attribute("source"))];
    }

    return sources;
}

template<unsigned int count>
static glm::vec<count, float> vec_at_index(
    uint32_t index, std::vector<float> const& points)
{
    glm::vec<count, float> result_vec {};
    for (int i = 0; i < count; i++) {
        result_vec[i] = points[index * count + i];
    }

    return result_vec;
}

static void compute_tangents(
    MeshBuilder& builder,
    std::vector<float> const& vertices,
    std::vector<float> const& texture_coords,
    std::vector<uint32_t> const& indices,
    int index)
{
    auto pos0 = vec_at_index<3>(indices[index + 3 * 0], vertices);
    auto pos1 = vec_at_index<3>(indices[index + 3 * 1], vertices);
    auto pos2 = vec_at_index<3>(indices[index + 3 * 2], vertices);
    auto uv0 = vec_at_index<2>(indices[index + 3 * 0 + 2], texture_coords);
    auto uv1 = vec_at_index<2>(indices[index + 3 * 1 + 2], texture_coords);
    auto uv2 = vec_at_index<2>(indices[index + 3 * 2 + 2], texture_coords);

    auto edge0 = pos1 - pos0;
    auto edge1 = pos2 - pos0;
    auto delta_uv0 = uv1 - uv0;
    auto delta_uv1 = uv2 - uv0;
    float const f = 1.0f / (delta_uv0.x * delta_uv1.y - delta_uv1.x * delta_uv0.y);

    auto tangent = (edge0 * delta_uv1.y - edge1 * delta_uv0.y) * f;
    for (int i = 0; i < 3; i++) {
        builder.add_tangent(tangent);
    }

    auto bitangent = (edge1 * delta_uv0.x - edge0 * delta_uv1.x) * f;
    for (int i = 0; i < 3; i++) {
        builder.add_bitangent(bitangent);
    }
}

static MeshBuilder load_mesh(pugi::xml_node const& mesh_node)
{
    MeshBuilder builder;
    auto sources = load_sources(mesh_node);

    std::vector<float> const* vertices = nullptr;
    std::vector<float> const* normals = nullptr;
    std::vector<float> const* texture_coords_0 = nullptr;
    std::vector<float> const* texture_coords_1 = nullptr;

    auto const& triangles_node = mesh_node.child("triangles");
    for (auto const& input_node : triangles_node.children("input")) {
        std::string const semantic = input_node.attribute("semantic").as_string();
        int const set = input_node.attribute("set").as_int();
        auto const& source = sources[parse_id_selector(input_node.attribute("source"))];

        if (semantic == "VERTEX") {
            vertices = &source.data;
        } else if (semantic == "NORMAL") {
            normals = &source.data;
        } else if (semantic == "TEXCOORD" && set == 0) {
            texture_coords_0 = &source.data;
        } else if (semantic == "TEXCOORD" && set == 1) {
            texture_coords_1 = &source.data;
        }
    }

    auto indicies = load_int_array(triangles_node.child("p"));
    uint32_t index_count = 0;
    for (int i = 0; i < indicies.size(); i += 3 * 3) {
        for (int j = 0; j < 3; j++) {
            auto index = i + j * 3;
            builder.add_vertex(vec_at_index<3>(indicies[index + 0], *vertices));
            builder.add_normal(vec_at_index<3>(indicies[index + 1], *normals));

            auto uv0 = vec_at_index<2>(indicies[index + 2], *texture_coords_0);
            if (texture_coords_1) {
                builder.add_uv01(uv0, vec_at_index<2>(indicies[index + 2], *texture_coords_1));
            } else {
                builder.add_uv0(uv0);
            }
        }

        compute_tangents(builder,
            *vertices, *texture_coords_0, indicies, i);

        builder.add_indicies({ index_count, index_count + 1, index_count + 2 });
        index_count += 3;
    }

    return builder;
}

static std::map<std::string, MeshBuilder> load_meshes(
    pugi::xml_node const& meshes_node)
{
    std::map<std::string, MeshBuilder> meshes;
    for (auto const& geometry_node : meshes_node.children("geometry")) {
        auto const& id = geometry_node.attribute("id").as_string();
        auto mesh = load_mesh(geometry_node.child("mesh"));
        meshes[id] = mesh;
    }

    return meshes;
}

static std::map<std::string, std::shared_ptr<Texture>> load_images(
    AssetRepository const& assets,
    pugi::xml_node const& images_node)
{
    std::map<std::string, std::shared_ptr<Texture>> images;
    for (auto const& image_node : images_node.children("image")) {
        auto const* id = image_node.attribute("id").as_string();
        auto const* file_path = image_node.child("init_from").text().as_string();
        auto texture = ImageTexture::construct(assets, file_path);
        images[id] = texture;
    }

    return images;
}

static std::map<std::string, std::shared_ptr<Texture>> load_textures(
    std::map<std::string, std::shared_ptr<Texture>> const& image_library,
    pugi::xml_node const& profile_node)
{
    std::map<std::string, std::shared_ptr<Texture>> textures;
    for (auto const& param_node : profile_node.children("newparam")) {
        auto const& sid = param_node.attribute("sid").as_string();
        if (param_node.child("surface")) {
            auto const& image_id = param_node
                                       .child("surface")
                                       .child("init_from")
                                       .text()
                                       .as_string();

            if (!image_library.contains(image_id)) {
                std::cerr << "Warning: Image '" << image_id << "', not found\n";
                continue;
            }
            textures[sid] = image_library.at(image_id);
        } else if (param_node.child("sampler2D")) {
            auto const& texture_id = param_node
                                         .child("sampler2D")
                                         .child("source")
                                         .text()
                                         .as_string();

            if (!textures.contains(texture_id)) {
                std::cerr << "Warning: Texture '" << texture_id << "', not found\n";
                continue;
            }
            textures[sid] = textures[texture_id];
        }
    }

    return textures;
}

static glm::vec3 load_color(pugi::xml_node const& color_node)
{
    auto float_array = load_float_array(color_node);
    return { float_array[0], float_array[1], float_array[2] };
}

static void load_lambert(
    AssetRepository const& assets,
    Material& material,
    std::map<std::string, std::shared_ptr<Texture>> texture_library,
    pugi::xml_node const& lambert_node)
{
    auto const& diffuse_node = lambert_node.child("diffuse");
    if (diffuse_node.child("texture")) {
        auto const* texture_id = diffuse_node
                                     .child("texture")
                                     .attribute("texture")
                                     .as_string();

        if (!texture_library.contains(texture_id)) {
            std::cerr << "Warning: Texture '" << texture_id << "' not found\n";
        } else {
            material.diffuse_map = texture_library[texture_id];
        }
    } else if (diffuse_node.child("color")) {
        // FIXME: Extremely hacky
        material.diffuse_map = ImageTexture::construct(assets, "/textures/bumper/white.jpg");
        material.color = load_color(diffuse_node
                                        .child("color"));
    }

    if (lambert_node.child("emission")) {
        material.emission_color = load_color(lambert_node
                                                 .child("emission")
                                                 .child("color"));
    }

    if (lambert_node.child("specular")) {
        auto const& specular_node = lambert_node.child("specular");
        material.specular_color = load_color(specular_node.child("color"));

        if (specular_node.child("float")) {
            material.specular_focus = specular_node
                                          .child("float")
                                          .text()
                                          .as_float();
        }
    }

    if (lambert_node.child("reflectivity")) {
        material.specular_color = glm::vec3(1.0)
            * lambert_node.child("reflectivity").child("float").text().as_float();
    }

    if (lambert_node.child("metallic")) {
        material.metallic = lambert_node
                                .child("metallic")
                                .child("float")
                                .text()
                                .as_float();
    }
}

static void load_extra(
    Material& material,
    std::map<std::string, std::shared_ptr<Texture>> texture_library,
    pugi::xml_node const& extra_node)
{
    auto const& technique_node = extra_node.child("technique");

    if (technique_node.child("bump")) {
        auto const& bump = technique_node.child("bump");
        auto const& texture_id = bump
                                     .child("texture")
                                     .attribute("texture")
                                     .as_string();

        if (!texture_library.contains(texture_id)) {
            std::cerr << "Warning: Texture '" << texture_id << "' not found\n";
        } else {
            material.normal_map = texture_library[texture_id];
        }

        if (bump.child("float")) {
            material.normal_map_strength = bump.child("float")
                                               .text()
                                               .as_float();
        }
    }

    if (technique_node.child("lightmap")) {
        auto const& texture_id = technique_node
                                     .child("lightmap")
                                     .child("texture")
                                     .attribute("texture")
                                     .as_string();

        if (!texture_library.contains(texture_id)) {
            std::cerr << "Warning: Texture '" << texture_id << "' not found\n";
        } else {
            material.light_map = texture_library[texture_id];
        }
    }
}

static std::map<std::string, Material> load_effects(
    AssetRepository const& assets,
    std::map<std::string, std::shared_ptr<Texture>> const& image_library,
    pugi::xml_node const& effects_node)
{
    std::map<std::string, Material> effects;
    for (auto const& effect_node : effects_node.children("effect")) {
        auto const* id = effect_node.attribute("id").as_string();
        auto const& profile_node = effect_node.child("profile_COMMON");
        auto texture_library = load_textures(image_library, profile_node);

        Material material;
        auto const& technique_node = profile_node.child("technique");

        if (technique_node.child("lambert")) {
            load_lambert(assets, material, texture_library,
                technique_node.child("lambert"));
        }

        if (technique_node.child("extra")) {
            load_extra(material, texture_library,
                technique_node.child("extra"));
        }

        effects[id] = material;
    }

    return effects;
}

static std::map<std::string, Material> load_materials(
    std::map<std::string, Material> const& effect_library,
    pugi::xml_node const& materials_node)
{
    std::map<std::string, Material> materials;
    for (auto const& material_node : materials_node.children("material")) {
        auto const* id = material_node.attribute("id").as_string();
        auto effect_id = material_node
                             .child("instance_effect")
                             .attribute("url");

        auto const& effect = effect_library.at(parse_id_selector(effect_id));
        materials[id] = effect;
    }

    return materials;
}

static glm::vec3 load_vec3(pugi::xml_node const& vec3_node)
{
    auto float_array = load_float_array(vec3_node);
    return { float_array[0], float_array[1], float_array[2] };
}

static glm::vec4 load_vec4(pugi::xml_node const& vec4_node)
{
    auto float_array = load_float_array(vec4_node);
    return { float_array[0], float_array[1], float_array[2], float_array[3] };
}

static glm::vec3 load_rotation(pugi::xml_node const& node_node)
{
    glm::vec3 rotation { 0, 0, 0 };
    for (auto const& rotate_node : node_node.children("rotate")) {
        auto data = load_vec4(rotate_node);
        rotation += glm::vec3(data.x, data.y, data.z) * glm::radians(data.w);
    }

    return rotation;
}

GameObject* ColladaLoader::open(
    GameObject& parent, AssetRepository const& assets, std::string_view model_name,
    std::function<void(GameObject&, Engine::MeshBuilder&, ModelMetaData)> const& on_object)
{
    pugi::xml_document doc;

    auto result = doc.load(*assets.open(model_name));
    if (!result) {
        std::cerr << "Error: Parsing Collada file: " << result.description() << "\n";
        return nullptr;
    }

    auto const& root = doc.child("COLLADA");
    auto image_library = load_images(assets, root.child("library_images"));
    auto effect_library = load_effects(assets, image_library, root.child("library_effects"));
    auto material_library = load_materials(effect_library, root.child("library_materials"));
    auto mesh_library = load_meshes(root.child("library_geometries"));

    auto& model_object = parent.add_child();
    auto const& visual_scenes_node = root.child("library_visual_scenes");
    for (auto const& visual_scene_node : visual_scenes_node.children("visual_scene")) {
        for (auto const& node_node : visual_scene_node.children("node")) {
            auto const* name = node_node.attribute("name").value();

            auto const& instance_geometry_node = node_node.child("instance_geometry");
            auto const& mesh_id = instance_geometry_node.attribute("url");
            auto const& material_id = instance_geometry_node
                                          .child("bind_material")
                                          .child("technique_common")
                                          .child("instance_material")
                                          .attribute("target");

            auto& mesh = mesh_library[parse_id_selector(mesh_id)];
            auto& material = material_library[parse_id_selector(material_id)];
            auto& game_object = model_object.add_child();
            on_object(game_object, mesh, ModelMetaData {
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

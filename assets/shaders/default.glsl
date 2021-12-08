#shader vertex
#version 400
in vec3 position;
in vec3 normal;
in vec4 uv01;
in vec3 tangent;
in vec3 bitangent;
out vec2 v_uv0;
out vec2 v_uv1;
out vec3 v_normal;
out vec4 v_world_position;
out mat3 v_tbn;

uniform mat4 model_matrix;
uniform mat4 mvp;

void main()
{
    vec3 t = normalize(vec3(model_matrix * vec4(tangent, 0.0)));
    vec3 b = normalize(vec3(model_matrix * vec4(bitangent, 0.0)));
    vec3 n = normalize(vec3(model_matrix * vec4(normal, 0.0)));
    v_tbn = mat3(t, b, n);

    v_uv0 = uv01.xy;
    v_uv1 = uv01.zw;
    v_normal = normal;
    v_world_position = model_matrix * vec4(position, 1);
    gl_Position = mvp * vec4(position, 1);
}

#shader fragment
#version 400
in vec2 v_uv0;
in vec2 v_uv1;
in vec3 v_normal;
in vec4 v_world_position;
in mat3 v_tbn;
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 LightColor;

uniform sampler2D diffuse_map;
uniform sampler2D normal_map;
uniform sampler2D light_map;
uniform samplerCube sky_box;
uniform vec3 camera_position;
uniform vec3 color;
uniform vec3 specular_color;
uniform vec3 emission_color;
uniform float normal_map_strength;
uniform float specular_focus;
uniform float metallic;
uniform bool has_light_map;

struct PointLight
{
    vec3 position;
    vec3 color;
};

uniform PointLight point_lights[10];
uniform int point_light_count;

void calculate_directional_light(
    vec3 normal, vec3 view_direction,
    inout vec3 diffuse_light, inout vec3 specular_light)
{
    const vec3 sun_color = vec3(0, 0, 0);

    vec3 light_direction = normalize(vec3(0.4, -1, 0));
    float intensity = 0.3;

    float angle_from_light = dot(-normal, light_direction);
    diffuse_light += sun_color * max(angle_from_light, 0.0) * intensity;

    vec3 reflect_direction = reflect(light_direction, -normal);
    specular_light += sun_color * pow(max(dot(view_direction, reflect_direction), 0.0), specular_focus) * intensity;
}

void calculate_point_lights(
    vec3 normal, vec3 view_direction,
    inout vec3 diffuse_light, inout vec3 specular_light)
{
    for (int i = 0; i < point_light_count; i++)
    {
        PointLight light = point_lights[i];
        vec3 to_light_direction = -normalize(light.position - v_world_position.xyz);
        float distance_to_light = length(light.position - v_world_position.xyz);
        float att_factor = 1.0 + (0.04 * distance_to_light) + (0.018 * (distance_to_light*distance_to_light));
        float angle_from_light = dot(-normal, to_light_direction);
        diffuse_light += light.color * (max(angle_from_light, 0.0) / att_factor);

        vec3 reflect_direction = reflect(to_light_direction, normal);
        float specular_intensity = pow(max(dot(view_direction, reflect_direction), 0.0), specular_focus);
        specular_light += light.color * (specular_intensity / att_factor);
    }
}

vec3 calculate_metallic_color(vec3 normal, vec3 view_direction)
{
    vec3 reflect_direction = reflect(-view_direction, normal);
    return texture(sky_box, reflect_direction).rgb;
}

void main()
{
    const float ambaint_light = 0.05;

    // Compute normal map
    vec3 normal = texture2D(normal_map, v_uv0).xyz;
    normal = mix(vec3(0, 0, 1), normal * 2.0 - 1.0, normal_map_strength);
    normal = normalize(v_tbn * normal);

    // Direction pointing to the camera
    vec3 view_direction = normalize(camera_position - v_world_position.xyz);
    view_direction = vec3(-view_direction.x, view_direction.y, -view_direction.z);

    // Compute dynamic lighting
    vec3 diffuse_light = vec3(0.0);
    vec3 specular_light = vec3(0.0);
    calculate_directional_light(normal, view_direction, diffuse_light, specular_light);
    calculate_point_lights(normal, view_direction, diffuse_light, specular_light);

    // Apply light map, if we have one
    if (has_light_map)
        diffuse_light *= texture2D(light_map, v_uv1).rgb;

    // Compute final pixel color
    vec3 metallic_color = calculate_metallic_color(normal, view_direction);
    vec3 diffuse_color = mix(texture2D(diffuse_map, v_uv0).rgb, metallic_color, metallic);
    vec3 final_color = 
        color * diffuse_color * max(diffuse_light, vec3(ambaint_light)) +
        specular_color * min(specular_light, vec3(1.0)) + 
        emission_color;
    FragColor = vec4(final_color, 1.0);
    LightColor = vec4(emission_color, 1.0);
}


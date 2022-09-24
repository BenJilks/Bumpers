#shader vertex
#version 300 es
precision highp float;
in vec3 position;
in vec3 cube_texture_coord;
out vec3 v_cube_texture_coord;

uniform mat4 mvp;

void main()
{
    v_cube_texture_coord = cube_texture_coord;
    gl_Position = mvp * vec4(position, 1.0);
}

#shader fragment
#version 300 es
precision highp float;
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 LightColor;
in vec3 v_cube_texture_coord;

uniform samplerCube diffuse_map;

void main()
{
    FragColor = texture(diffuse_map, v_cube_texture_coord);
    LightColor = vec4(0.0);
}

#shader vertex
#version 400
in vec3 position;
in vec2 texture_coord;
out vec2 v_texture_coord;

uniform mat4 mvp;

void main()
{
    v_texture_coord = texture_coord;
    gl_Position = mvp * vec4(position, 1);
}

#shader fragment
#version 400
in vec2 v_texture_coord;
out vec4 FragColor;

uniform sampler2D diffuse;
uniform vec4 color;

void main()
{
    FragColor = texture2D(diffuse, v_texture_coord) * color;
}


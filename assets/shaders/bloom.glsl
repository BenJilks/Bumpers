#shader vertex
#version 400
in vec3 position;
in vec2 texture_coord;
out vec2 v_texture_coord;

void main()
{
    v_texture_coord = texture_coord;
    gl_Position = vec4(position, 1);
}

#shader fragment
#version 400
out vec4 FragColor;
in vec2 v_texture_coord;

uniform sampler2D standard_view;
uniform sampler2D light_only_view;

void main()
{
    const float exposure = 1;
    const float gamma = 2.2;

    vec3 color = texture2D(standard_view, v_texture_coord).rgb;
    vec3 light = texture2D(light_only_view, v_texture_coord).rgb;
    FragColor = vec4(color + light, 1.0);
}

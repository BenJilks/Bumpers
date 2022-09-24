#shader vertex
#version 300 es
precision highp float;
in vec3 position;
in vec2 texture_coord;
out vec2 v_texture_coord;

void main()
{
    v_texture_coord = texture_coord;
    gl_Position = vec4(position, 1);
}

#shader fragment
#version 300 es
precision highp float;
out vec4 FragColor;
in vec2 v_texture_coord;

uniform vec2 standard_view_scale;
uniform sampler2D standard_view;

uniform vec2 light_only_view_scale;
uniform sampler2D light_only_view;

void main()
{
    const float exposure = 1.0;
    const float gamma = 2.2;

    vec3 color = texture(standard_view, v_texture_coord * standard_view_scale).rgb;
    vec3 light = texture(light_only_view, v_texture_coord * light_only_view_scale).rgb;
    FragColor = vec4(color + light, 1.0);
}

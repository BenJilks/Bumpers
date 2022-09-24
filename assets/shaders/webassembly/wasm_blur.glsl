#shader vertex
#version 300 es
precision highp float;
in vec3 position;
in vec2 texture_coord;
out vec2 v_texture_coord;

uniform vec2 view_scale;

void main()
{
    v_texture_coord = texture_coord * view_scale;
    gl_Position = vec4(position, 1);
}

#shader fragment
#version 300 es
precision highp float;
out vec4 FragColor;
in vec2 v_texture_coord;

uniform sampler2D view;
uniform vec2 screen_size;
uniform vec2 direction;

const float weight[9] = float[]
(
    0.17619, 0.16017, 0.12013, 0.07392, 0.03696,
    0.01478, 0.00462, 0.00108, 0.00018
);

void main()
{
    vec2 texal_size = 1.0 / screen_size;
    vec3 color = texture(view, v_texture_coord).rgb * weight[0];
    for(int i = 1; i < 9; ++i)
    {
        vec2 offset      = texal_size * float(i)*2.0         * direction;
        vec2 offset_next = texal_size * (float(i)*2.0 + 1.0) * direction;
        color += (texture(view, v_texture_coord + offset).rgb * weight[i] +
                  texture(view, v_texture_coord + offset_next).rgb * weight[i]) / 2.0;
        color += (texture(view, v_texture_coord - offset).rgb * weight[i] +
                  texture(view, v_texture_coord - offset_next).rgb * weight[i]) / 2.0;
    }

    FragColor = vec4(color, 1.0);
}

#version 460 core

layout (location = 0) in vec3 vertex_pos;
layout (location = 1) in vec4 vertex_color;
layout (location = 2) in vec2 vertex_texture;

uniform float system_tick;
uniform mat4 system_mvp_matrix;

out vec4 ver_blend_color; 
out vec2 ver_texture_coord;

void main()
{
    gl_Position = system_mvp_matrix * vec4(vertex_pos, 1);
    
    ver_blend_color   = vertex_color;
    ver_texture_coord = vertex_texture;
}
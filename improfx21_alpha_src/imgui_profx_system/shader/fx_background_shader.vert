#version 460 core
layout (location = 0) in vec3 vertex_pos;
layout (location = 1) in vec4 vertex_color;
layout (location = 2) in vec2 vertex_texture;

uniform float FxTime;
uniform mat4  FxMatMvp;

out vec4 FxColor; 
out vec2 FxCoord;

void main()
{
	gl_Position = FxMatMvp * vec4(vertex_pos, 1);
	FxColor     = vertex_color;
	FxCoord     = vertex_texture;
}
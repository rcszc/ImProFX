// framework_particle_script.
#include "framework_particle.h"

namespace particle_shader_script {
	// default_particle vertex shader code.
	const char* FxShaderDefaultVert = R"(
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
)";
	  
	// default_particle fragment shader code.
	const char* FxShaderDefaultFrag = R"(
#version 460 core

in vec4 FxColor;
in vec2 FxCoord;

out vec4 FragColor;

#define FxResolution gl_FragCoord.xy
uniform float FxTime;

const vec2 CenterPoint = vec2(0.5, 0.5);

void main() {
	float CenterLength = distance(FxCoord, CenterPoint);
    FragColor = FxColor * vec4(1.0, 1.0, 1.0, (1.0 / CenterLength * 0.16));
}
)";
}
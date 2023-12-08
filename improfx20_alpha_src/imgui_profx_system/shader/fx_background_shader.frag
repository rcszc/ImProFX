#version 460 core

in vec4 FxColor;
in vec2 FxCoord;

uniform vec2  FxWinSize;
uniform float FxTime;

uniform float brightness;
uniform float speed;

out vec4 FragColor;

const vec2 iResolution = vec2(1562.5, 1000.0);

// 来源: https://www.shadertoy.com/view/4scGWj

const float iTime = FxTime * speed * 0.72;

float gTime = 0.0;
const float REPEAT = 5.0;

mat2 rot(float a) {
	float c = cos(a), s = sin(a);
	return mat2(c, s, -s, c);
}

float sdBox(vec3 p, vec3 b) {
	vec3 q = abs(p) - b;
	return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

float box(vec3 pos, float scale) {
	pos *= scale;
	float base = sdBox(pos, vec3(0.4, 0.4, 0.1)) / 1.5;
	pos.xy *= 5.0;
	pos.y -= 3.5;
	pos.xy *= rot(0.75);
	float result = -base;
	return result;
}

float box_set(vec3 pos, float iTime) {
	vec3 pos_origin = pos;

	pos = pos_origin;
	pos.y += sin(gTime * 0.4) * 2.5;
	pos.xy *= rot(0.8);
	float box1 = box(pos, 2.0 - abs(sin(gTime * 0.4)) * 1.5);

	pos = pos_origin;
	pos.y -=sin(gTime * 0.4) * 2.5;
	pos.xy *= rot(0.8);
	float box2 = box(pos, 2.0 - abs(sin(gTime * 0.4)) * 1.5);

	pos = pos_origin;
	pos.x +=sin(gTime * 0.4) * 2.5;
	pos.xy *= rot(0.8);
	float box3 = box(pos, 2.0 - abs(sin(gTime * 0.4)) * 1.5);	

	pos = pos_origin;
	pos.x -=sin(gTime * 0.4) * 2.5;
	pos.xy *= rot(0.8);
	float box4 = box(pos, 2.0 - abs(sin(gTime * 0.4)) * 1.5);

	pos = pos_origin;
	pos.xy *= rot(0.8);
	float box5 = box(pos, 0.5) * 6.0;	
	pos = pos_origin;
	float box6 = box(pos, 0.5) * 6.0;

	float result = max(max(max(max(max(box1, box2), box3), box4), box5), box6);
	return result;
}

float map(vec3 pos, float iTime) {
	vec3 pos_origin = pos;
	float box_set1 = box_set(pos, iTime);

	return box_set1;
}

void main() {

	vec2 p = (gl_FragCoord.xy * 2.0 - iResolution.xy) / min(iResolution.x, iResolution.y);
	vec3 ro = vec3(0.0, -0.2, iTime * 4.0);
	vec3 ray = normalize(vec3(p, 1.5));
	ray.xy = ray.xy * rot(sin(iTime * 0.03) * 5.0);
	ray.yz = ray.yz * rot(sin(iTime * 0.05) * 0.2);
	float t = 0.1;
	vec3 col = vec3(0.0);
	float ac = 0.0;

	for (int i = 0; i < 128; i++){
		vec3 pos = ro + ray * t;
		pos = mod(pos - 2.0, 4.0) - 2.0;
		gTime = iTime - float(i) * 0.01;
		
		float d = map(pos, iTime);

		d = max(abs(d), 0.01);
		ac += exp(-d * 23.0);

		t += d * 0.55;
	}

	col = vec3(ac * 0.02); 
	col +=vec3(0.5 * abs(sin(iTime)), 0.0, 0.5 + sin(iTime) * 0.2);

	FragColor = vec4(col, 0.5 * brightness);
}
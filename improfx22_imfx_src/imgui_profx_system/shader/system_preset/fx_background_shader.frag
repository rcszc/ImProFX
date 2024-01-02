#version 460 core

in vec4 FxColor;
in vec2 FxCoord;

uniform vec2  FxWinSize;
uniform float FxTime;

uniform float brightness;
uniform float speed;

out vec4 FragColor;

const vec2 iResolution = vec2(1000.0, 1000.0);

// system default shader.
// 来源: https://www.shadertoy.com/view/WdlyRS
// 修改: RCSZ 2023_12_16 (Update: 2024.01.01) 

#define iTime FxTime * speed
#define t     FxTime * speed * 2.0
#define SIZE 30.0

#define col1 vec3(0.0, 0.55, 1.0)
#define col2 vec3(0.0, 1.0, 0.55)

vec2 ran(vec2 uv) {
    uv *= vec2(dot(uv, vec2(127.1, 311.7)),dot(uv, vec2(227.1, 521.7)));
    return 1.0 - fract(tan(cos(uv) * 123.6) * 3533.3) * fract(tan(cos(uv) * 123.6) * 3533.3);
}
vec2 pt(vec2 id) {
    return sin(t * (ran(id + 0.5) - 0.5) + ran(id - 20.1) * 8.0) * 0.5;
}


void main()
{
    vec2 uv = (gl_FragCoord.xy - 0.5 * iResolution.xy) / iResolution.x;
    vec2 off = iTime / vec2(50.0, 30.0);
    uv += off;
    uv *= SIZE;
    
    vec2 gv = fract(uv) - 0.5;
    vec2 id = floor(uv);
    
    float mindist = 1e9;
    vec2 vorv;
    for(float i = -1.0; i <= 1.0; i++) {
        for(float j = -1.0; j <= 1.0; j++) {

            vec2 offv = vec2(i,j);
            float dist = length(gv + pt(id + offv) - offv);

            if(dist < mindist){
                mindist = dist;
                vorv = (id + pt(id + offv) + offv) / SIZE - off;
            }
        }
    }
    vec3 color = mix(col1, col2, clamp(vorv.x * 2.2 + vorv.y, -1.0, 1.0) * 0.5 + 0.5) * brightness;
    
    FragColor = vec4(color, 0.82);
}
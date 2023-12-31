#version 460 core

in vec4 FxColor;
in vec2 FxCoord;

uniform vec2  FxWinSize;
uniform float FxTime;

out vec4 FragColor;

const vec2 iResolution = vec2(1000.0, 1000.0);

// À´Ô´: https://www.shadertoy.com/view/ctyyRG

//https://iquilezles.org/articles/palettes/
vec3 palette(float t) {

    vec3 a = vec3(0.5, 0.5, 0.5);
    vec3 b = vec3(0.5, 0.5, 0.5);
    vec3 c = vec3(1.0, 1.0, 1.0);
    vec3 d = vec3(0.263,0.416,0.557);

    return a + b * cos(6.28318 * (c * t + d));
}

//https://www.shadertoy.com/view/mtyGWy
void main() {

    vec2 uv = (gl_FragCoord.xy * 2.0 - iResolution.xy) / iResolution.y;
    uv.x -= 0.72;
    vec2 uv0 = uv;
    vec3 finalColor = vec3(0.0);
    
    for (float i = 0.0; i < 4.0; i++) {
        uv = fract(uv * 1.5) - 0.5;

        float d = length(uv) * exp(-length(uv0));

        vec3 col = palette(length(uv0) + i * 0.4 + FxTime * 0.4);

        d = sin(d * 8.0 + FxTime) / 8.0;
        d = abs(d);

        d = pow(0.01 / d, 1.2);

        finalColor += col * d;
    }
    FragColor = vec4(finalColor, 1.0);
}
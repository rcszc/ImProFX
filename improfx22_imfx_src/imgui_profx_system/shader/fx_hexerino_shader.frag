#version 460 core

in vec4 FxColor;
in vec2 FxCoord;

uniform vec2  FxWinSize;
uniform float FxTime;

uniform float lightning;

uniform sampler2D logo_texture;

out vec4 FragColor;

const vec2 iResolution = vec2(1562.5, 1000.0);

// À´Ô´: https://www.shadertoy.com/view/ctyfRh

#define H(a) (cos(radians(vec3(0, 60, 120))+(a)*6.2832)*.5+.5)  // hue

void main() {

    vec2 U = gl_FragCoord.xy;
    vec3 iMouse = vec3(FxTime * 32.0, FxTime * 32.0, 1.0);

    float t = FxTime / 300.0, // num sec between ints
          n = 6.2832,         // pi2
          s = 6.,             // scale
          i, p, r, k;
    
    vec2 R = iResolution.xy,
         m = (iMouse.xy - R/2.)/R.y*s,  // mouse coords
         v = (U+U-R)/R.y;               // cartesian coords
    
    if (iMouse.z < 1.) m = s*vec2(sin(t*n)*2., sin(t*n*2.)); // fig-8 movement
    k = dot(v, v); // transformed coords
    r = length(v); // screen y radius
    
    vec3 c = vec3(0),
         d = H(t*10. + r/2.)*.07; // color
    
    for(i = 0.; i < 1.; i += .2)
    {
        vec2 u = i*v*s/k - m,       // transformed coords
             o = vec2(.5, .866),    // hex offset
             a = mod(u,   o+o) - o, // grid 1
             b = mod(u-o, o+o) - o, // grid 2
             h = dot(a,a) < dot(b,b) ? a : b, // combine grids for hex tile
             k = abs(h);
        
        p = pow(length(u-h), 2.)*t; // pattern (radial moire)
        
        c = max(c, (1.-max(k.x, dot(k, o))*3.) // hex tile
          * (sin(p*n)/2.+1.)     // brightness pattern
          * (H(p+.5)*.5+.5)      // color pattern
          * min(r*sqrt(r), 1./r) // darken center & edges
          * pow(i, .1)           // darken hex near camera
          * (1.3-r)              // flip outer
          + d);                  // more color
    }
    
    vec4 color = texture(logo_texture, FxCoord);
    vec4 out_color = vec4(0.0);
    if (color.w == 1.0)
        out_color = vec4(c + c * c * 8.0, 0.72) + texture(logo_texture, FxCoord) * 0.32;
    
    FragColor = out_color;
}
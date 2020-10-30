#version 400

in vec2 v_texcoord;
in vec3 v_normal;

uniform sampler2D s_texture;
uniform vec3 cam_forward;
uniform vec4 tint;
uniform float hue;
uniform float saturation;
uniform vec3 sky_dir;
uniform vec3 sky_color;
uniform vec3 ambient_color;
uniform bool enable_lighting;

layout(location = 0) out vec4 outColor;

// http://lolengine.net/blog/2013/07/27/rgb-to-hsv-in-glsl
vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = c.g < c.b ? vec4(c.bg, K.wz) : vec4(c.gb, K.xy);
    vec4 q = c.r < p.x ? vec4(p.xyw, c.r) : vec4(c.r, p.yzx);
    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main()
{
    vec4 color = texture2D(s_texture, v_texcoord) * tint;

    if (color.a < 1.0/255.0) discard;

    vec3 hsv = rgb2hsv(color.rgb);

    hsv.x += hue;
    hsv.y *= saturation;

    color.rgb = hsv2rgb(hsv);

    if (enable_lighting) {
        vec3 diffuse = 0.6 * max(dot(v_normal, sky_dir), 0.0) * sky_color;
        vec3 ambient = 0.4 * ambient_color;
        color.rgb = color.rgb * (ambient + diffuse);
    }

    outColor = color;
}

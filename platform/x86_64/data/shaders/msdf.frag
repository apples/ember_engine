#version 400

in vec2 v_texcoord;
in vec3 v_normal;

uniform sampler2D msdf;
uniform float pxRange;
uniform vec2 texSize;
uniform vec4 fgColor;

layout(location = 0) out vec4 outColor;

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

void main() {
    vec2 msdfUnit = pxRange/texSize;
    vec3 values = texture(msdf, v_texcoord).rgb;
    float sigDist = median(values.r, values.g, values.b) - 0.5;
    sigDist *= dot(msdfUnit, 0.5/fwidth(v_texcoord));
    float opacity = clamp(sigDist + 0.5, 0.0, 1.0);
    outColor = vec4(fgColor.rgb, fgColor.a*opacity);
}

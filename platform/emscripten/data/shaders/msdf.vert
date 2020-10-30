attribute vec3 VertexPosition;
attribute vec2 VertexTexCoord;
attribute vec3 VertexNormal;

varying vec2 v_texcoord;
varying vec3 v_normal;

uniform mat4 MVP;
uniform mat4 normal_mat;

void main()
{
    v_texcoord = VertexTexCoord;
    v_normal = vec3(normal_mat * vec4(VertexNormal, 0.0));
    gl_Position = MVP * vec4(VertexPosition, 1.0);
}

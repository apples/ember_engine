#version 400

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec2 VertexTexCoord;
layout(location = 2) in vec3 VertexNormal;

out vec2 v_texcoord;
out vec3 v_normal;

uniform mat4 MVP;
uniform mat4 normal_mat;

void main()
{
    v_texcoord = VertexTexCoord;
    v_normal = vec3(normal_mat * vec4(VertexNormal, 0.0));
    gl_Position = MVP * vec4(VertexPosition, 1.0);
}

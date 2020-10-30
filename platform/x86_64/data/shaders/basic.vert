#version 400

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec2 VertexTexCoord;
layout(location = 2) in vec3 VertexNormal;
layout(location = 3) in vec4 VertexBlendIndices;
layout(location = 4) in vec4 VertexBlendWeights;

out vec2 v_texcoord;
out vec3 v_normal;

uniform mat4 MVP;
uniform mat4 normal_mat;
uniform mat3 uvmat;
uniform bool Animated;
uniform mat4 Bones[32];

void main() {
    mat4 transform = MVP;

	if (Animated) {
        transform =
            MVP * (
                Bones[int(VertexBlendIndices[0])] * VertexBlendWeights[0] +
                Bones[int(VertexBlendIndices[1])] * VertexBlendWeights[1] +
                Bones[int(VertexBlendIndices[2])] * VertexBlendWeights[2] +
                Bones[int(VertexBlendIndices[3])] * VertexBlendWeights[3]);
    }

    v_texcoord = vec2(uvmat * vec3(VertexTexCoord, 1.0));
    v_normal = normalize(vec3(normal_mat * vec4(VertexNormal, 0.0)));
    gl_Position = transform * vec4(VertexPosition, 1.0);
}

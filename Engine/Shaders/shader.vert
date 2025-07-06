#version 450
#extension GL_EXT_buffer_reference : require

struct Vertex {
    vec3 pos;
    float xUV;
    vec3 normal;
    float yUV;
    vec3 colour;

};

layout(buffer_reference, std430, buffer_reference_align = 16) readonly buffer VertexBuffer{
    Vertex vertices[];
};

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} ubo;

layout( push_constant ) uniform constants
{
	mat4 modelMatrix;
    VertexBuffer vertexBuffer;
    float Ka;
    float Kd;
    float Ks;
} PushConstants;


layout(location = 0) out vec3 fragColour;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 surfaceNormal;
layout(location = 3) out float Ka;
layout(location = 4) out float Kd;
layout(location = 5) out float Ks;
layout(location = 6) out vec3 fragPos;

void main() {
    //Load vertex data from vertex bufferdevice adress
    Vertex v = PushConstants.vertexBuffer.vertices[gl_VertexIndex];

    gl_Position = ubo.proj * ubo.view * PushConstants.modelMatrix * vec4(v.pos, 1.0);

    fragColour = v.colour;
    fragTexCoord = vec2(v.xUV, v.yUV);
    surfaceNormal = v.normal;
    Ka = PushConstants.Ka;
    Kd = PushConstants.Kd;
    Ks = PushConstants.Ks;
    fragPos = vec3(PushConstants.modelMatrix * vec4(v.pos, 1.0)); //Multiply by model matrix to get in world coords for blinn-phong
}
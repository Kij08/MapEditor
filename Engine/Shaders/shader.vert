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
    bool isSelected;
} PushConstants;


layout(location = 0) out vec3 fragColourTint;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 surfaceNormal;
layout(location = 3) out vec3 fragPos;

void main() {
    //Load vertex data from vertex bufferdevice adress
    Vertex v = PushConstants.vertexBuffer.vertices[gl_VertexIndex];

    gl_Position = ubo.proj * ubo.view * PushConstants.modelMatrix * vec4(v.pos, 1.0);

    if(PushConstants.isSelected == true){
        fragColourTint = vec3(1, 0.6, 0);
    }
    else{
        fragColourTint = vec3(1, 1, 1);
    }
    fragTexCoord = vec2(v.xUV, v.yUV);
    surfaceNormal = v.normal;
    fragPos = vec3(PushConstants.modelMatrix * vec4(v.pos, 1.0)); //Multiply by model matrix to get in world coords for blinn-phong
}
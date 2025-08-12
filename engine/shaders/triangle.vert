#version 450

layout(binding = 0) uniform UniformBufferObject
{
    mat4 model;
    mat4 view;
    mat4 proj;
}ubo;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 color;
layout(location = 3) in vec2 uv;

layout(location = 0) out vec3 frag_color;

vec2 positions[3] = {
    vec2( 0.0, -0.6),
    vec2(-0.5,  0.6),
    vec2( 0.5,  0.6),
};

vec3 colors[3] = {
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0),
};

void main()
{
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);

    frag_color = colors[gl_VertexIndex];
}

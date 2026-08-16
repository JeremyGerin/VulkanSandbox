#version 450

layout(set = 0, binding = 0) uniform UBOColor {
    vec3 color;
} ubo;

layout(location = 0) in vec2 in_position;

layout(location = 0) out vec3 frag_color;

void main() {
    gl_Position = vec4(in_position, 0.0, 1.0);
    frag_color = ubo.color;
}
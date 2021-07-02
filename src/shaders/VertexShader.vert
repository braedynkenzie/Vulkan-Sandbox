#version 450 

layout(location = 0) in vec2 in_position;

layout(push_constant) uniform PushConstantData {
	vec2 offset;
	vec4 colour;
} pushConstantData;

void main() {
	gl_Position = vec4(in_position + pushConstantData.offset, 0.0, 1.0);
}
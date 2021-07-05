#version 450 

layout(push_constant) uniform PushConstantData {
	mat2 transform;
	vec2 offset;
	vec4 colour;
} pushConstantData;

layout(location = 0) out vec4 fragColour;

void main() {
	fragColour = pushConstantData.colour;
}
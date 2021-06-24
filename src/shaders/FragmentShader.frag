#version 450 

layout(location = 0) in vec4 in_colour;

layout(location = 0) out vec4 fragColour;

void main() {
	fragColour = in_colour;
}
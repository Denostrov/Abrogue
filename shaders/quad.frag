#version 450

layout(location = 0) in vec2 fragTexCoords;

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D texSampler;

void main()
{
	outColor = vec4(texture(texSampler, fragTexCoords).r, 0.0, 0.0, 1.0);
}
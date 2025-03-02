#version 450

layout(location = 0) in vec2 fragTexCoords;
layout(location = 1) flat in uvec2 fragColors;

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D texSampler;

vec4 getColor(in uint color)
{
	return vec4((color >> 24 & 255) / 255.0, (color >> 16 & 255) / 255.0, (color >> 8 & 255) / 255.0, (color & 255) / 255.0);
}

void main()
{
	vec4 color = getColor(fragColors[0]);
	vec4 backgroundColor = getColor(fragColors[1]);

	float threshold = texture(texSampler, fragTexCoords).r;
	outColor = mix(backgroundColor, color, threshold);
}
#version 450

layout(location = 0) in vec2 fragTexCoords;
layout(location = 1) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D texSampler;

void main()
{
	float threshold = texture(texSampler, fragTexCoords).r;
	if (threshold >= 0.75)
		outColor = vec4(fragColor, 1.0);
	else if (threshold >= 0.25)
		outColor = vec4(1.0, 0.0, 0.0, 1.0);
	else
		discard;
}
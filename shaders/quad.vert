#version 450

#extension GL_EXT_scalar_block_layout: require
#extension GL_EXT_buffer_reference : require
#extension GL_EXT_buffer_reference2 : require

layout (buffer_reference, scalar, buffer_reference_align = 4) readonly buffer QuadReference
{
	vec2 position;
	vec2 scale;
	vec2 rotation;
	uvec2 colors;
	uint glyphIndex;
};

layout (push_constant) uniform PushConstants
{
	QuadReference quadDataReference;
} pushConstants;

vec2 positions[4] = vec2[4](
	vec2(-1.0, -1.0),
	vec2(1.0, -1.0),
	vec2(-1.0, 1.0),
	vec2(1.0, 1.0)
);

layout(location = 0) out vec2 fragTexCoords;
layout(location = 1) out uvec2 fragColors;

void main()
{
	QuadReference quadData = pushConstants.quadDataReference[gl_InstanceIndex];

	vec2 position = positions[gl_VertexIndex];
	position *= quadData.scale;
	
	position = vec2(position.x * quadData.rotation.x - position.y * quadData.rotation.y, position.x * quadData.rotation.y + position.y * quadData.rotation.x);
	
	vec2 quadPosition = quadData.position * 2.0 - vec2(16.0 / 9.0, 1.0);
	position += quadPosition;
	gl_Position = vec4(position.x / 16.0 * 9.0, position.y, 0.0, 1.0);
	
	vec2 texPosition = positions[gl_VertexIndex];
	float yOffset = trunc(quadData.glyphIndex / 16.0);
	texPosition += vec2(1.0, 1.0);
	texPosition /= 2.0;
	texPosition *= 1.0 / 16.0;
	texPosition = vec2(texPosition.x * 30.0 / 32.0 + 1.0 / 16.0 / 32.0, texPosition.y * 62.0 / 64.0 + 1.0 / 16.0 / 64.0);
	texPosition = vec2(texPosition.x + 1.0 / 16.0 * (quadData.glyphIndex - yOffset * 16.0), texPosition.y + 1.0 / 16.0 * yOffset);
	fragTexCoords = texPosition;
	
	fragColors = quadData.colors;
}
#version 450

#extension GL_EXT_scalar_block_layout: require
#extension GL_EXT_buffer_reference : require
#extension GL_EXT_buffer_reference2 : require

layout (buffer_reference, scalar, buffer_reference_align = 4) readonly buffer QuadReference
{
	vec2 position;
	vec2 scale;
	uvec3 colors;
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
layout(location = 1) out uvec3 fragColors;

void main()
{
	QuadReference quadData = pushConstants.quadDataReference[gl_InstanceIndex];

	vec2 position = positions[gl_VertexIndex];
	gl_Position = vec4((position.x * quadData.scale.x + quadData.position.x) / 16.0 * 9.0, position.y * quadData.scale.y + quadData.position.y, 0.0, 1.0);
	
	float yOffset = trunc(quadData.glyphIndex / 16.0);
	position += vec2(1.0, 1.0);
	position /= 2.0;
	position *= 1.0 / 16.0;
	position = vec2(position.x * 30.0 / 32.0 + 1.0 / 16.0 / 32.0, position.y * 62.0 / 64.0 + 1.0 / 16.0 / 64.0);
	position = vec2(position.x + 1.0 / 16.0 * (quadData.glyphIndex - yOffset * 16.0), position.y + 1.0 / 16.0 * yOffset);
	fragTexCoords = position;
	
	fragColors = quadData.colors;
}
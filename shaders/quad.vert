#version 450

#extension GL_EXT_scalar_block_layout: require
#extension GL_EXT_buffer_reference : require
#extension GL_EXT_buffer_reference2 : require

layout (buffer_reference, scalar) readonly buffer QuadReference
{
	vec2 position;
	vec2 scale;
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

void main()
{
	QuadReference quadData = pushConstants.quadDataReference[gl_InstanceIndex];

	gl_Position = vec4((positions[gl_VertexIndex].x * quadData.scale.x + quadData.position.x) / 16.0 * 9.0, positions[gl_VertexIndex].y * quadData.scale.y + quadData.position.y, 0.0, 1.0);
}
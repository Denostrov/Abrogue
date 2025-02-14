#version 450

#extension GL_EXT_buffer_reference : require

layout (buffer_reference, scalar) readonly buffer QuadReference
{
	vec2 position;
};

layout (push_constant) uniform PushConstants
{
	QuadReference quadDataReference;
} pushConstants;

vec2 positions[4] = vec2[4](
	vec2(-0.5, -0.5),
	vec2(0.5, -0.5),
	vec2(-0.5, 0.5),
	vec2(0.5, 0.5)
);

void main()
{
	quadData = pushConstants.quadDataReference;

	gl_Position = vec4(positions[gl_VertexIndex].x + quadData[gl_InstanceID].x, positions[gl_VertexIndex].y + quadData[gl_InstanceID].y, 0.0, 1.0);
}
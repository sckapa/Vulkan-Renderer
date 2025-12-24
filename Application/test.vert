#version 460

struct VertexData
{
	float x, y, z;
	float u, v;
};

layout (binding = 0) readonly buffer Vertices { VertexData data[]; } in_vertices;
layout (binding = 1) readonly uniform UniformBuffer { mat4 wvp; } WVP;

void main() 
{
	VertexData vtx = in_vertices.data[gl_VertexIndex];

	vec3 pos = vec3(vtx.x, vtx.y, vtx.z);

	gl_Position = WVP.wvp * vec4( pos, 1.0 );
}
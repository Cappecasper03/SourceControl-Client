#version 460 core

layout( location = 0 ) in vec3 in_position;
layout( location = 1 ) in vec3 in_normal;
layout( location = 2 ) in vec3 in_tangent;
layout( location = 3 ) in vec3 in_bitangent;
layout( location = 4 ) in vec2 in_tex_coord;

layout( set = 0, binding = 0 ) uniform sVertexSceneUniforms
{
	mat4 view_projection;
}
IN_SCENE;

layout( push_constant ) uniform sPushConstant
{
	mat4 world_matrix;
}
PUSH_CONSTANT;

layout( location = 0 ) out vert_frag
{
	vec2 tex_coord_ts;
}
OUT;

void main()
{
	gl_Position      = IN_SCENE.view_projection * PUSH_CONSTANT.world_matrix * vec4( in_position, 1 );
	OUT.tex_coord_ts = in_tex_coord;
}
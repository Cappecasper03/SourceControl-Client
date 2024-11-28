#version 460 core

layout( location = 0 ) in vec3 in_position;
layout( location = 1 ) in vec3 in_normal;
layout( location = 2 ) in vec3 in_tangent;
layout( location = 3 ) in vec3 in_bitangent;
layout( location = 4 ) in vec2 in_tex_coord;

out vert_frag
{
	vec3 position_ws;
	vec2 tex_coord_ts;
	mat3 tbn_ws;
}
OUT;

uniform mat4 u_world_matrix;
uniform mat4 u_view_projection_matrix;

void main()
{
	const vec3 position_ws = vec4( u_world_matrix * vec4( in_position, 1 ) ).rgb;

	gl_Position      = u_view_projection_matrix * u_world_matrix * vec4( in_position, 1 );
	OUT.position_ws  = position_ws;
	OUT.tex_coord_ts = in_tex_coord;
	OUT.tbn_ws       = mat3( in_tangent, in_bitangent, in_normal );
}
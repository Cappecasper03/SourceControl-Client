#version 460 core

layout( location = 0 ) in vec3 in_position;
layout( location = 1 ) in vec3 in_normal;
layout( location = 2 ) in vec3 in_tangent;
layout( location = 3 ) in vec3 in_bitangent;
layout( location = 4 ) in vec2 in_tex_coord;

out vert_frag
{
	vec2 tex_coord_ts;
}
OUT;

uniform mat4 u_world_matrix;
uniform mat4 u_view_projection_matrix;

void main()
{
	gl_Position      = u_view_projection_matrix * u_world_matrix * vec4( in_position, 1 );
	OUT.tex_coord_ts = in_tex_coord;
}
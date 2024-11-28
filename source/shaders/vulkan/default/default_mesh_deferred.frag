#version 460 core

layout( location = 0 ) in vert_frag
{
	vec3 position_ws;
	vec2 tex_coord_ts;
	mat3 tbn_ws;
}
IN;

layout( set = 1, binding = 0 ) uniform sampler2D in_color_texture;
layout( set = 1, binding = 1 ) uniform sampler2D in_normal_texture;
layout( set = 1, binding = 2 ) uniform sampler2D in_specular_texture;

layout( location = 0 ) out vec3 out_position;
layout( location = 1 ) out vec3 out_normal;
layout( location = 2 ) out vec4 out_color_specular;

void main()
{
	const vec3 normal_map_ts = texture( in_normal_texture, IN.tex_coord_ts ).xyz * 2 - 1;
	const vec3 normal_ws     = ( normalize( IN.tbn_ws * normal_map_ts ) + 1 ) / 2;

	out_position           = IN.position_ws;
	out_normal             = normal_ws;
	out_color_specular.rgb = texture( in_color_texture, IN.tex_coord_ts ).rgb;
	out_color_specular.a   = texture( in_specular_texture, IN.tex_coord_ts ).r;
}
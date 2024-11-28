#version 460 core

layout( location = 0 ) in vert_frag
{
	vec2 tex_coord_ts;
}
IN;

layout( set = 1, binding = 0 ) uniform sampler2D in_position_texture;
layout( set = 1, binding = 1 ) uniform sampler2D in_normal_texture;
layout( set = 1, binding = 2 ) uniform sampler2D in_color_specular_texture;

layout( location = 0 ) out vec4 out_color;

void main()
{
	const vec4  position       = texture( in_position_texture, IN.tex_coord_ts );
	const vec4  normal         = texture( in_normal_texture, IN.tex_coord_ts ) * 2 - 1;
	const vec4  color_specular = texture( in_color_specular_texture, IN.tex_coord_ts );
	const vec3  color          = color_specular.rgb;
	const float specular       = color_specular.a;

	out_color = vec4( color, 1 );
}
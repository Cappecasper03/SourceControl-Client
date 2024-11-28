#version 460 core

in vert_frag
{
	vec2 tex_coord_ts;
}
IN;

layout( location = 0 ) out vec4 out_color;

uniform sampler2D u_color_texture;

void main()
{
	const vec4 texture_color = texture( u_color_texture, IN.tex_coord_ts );

	out_color = texture_color;
}
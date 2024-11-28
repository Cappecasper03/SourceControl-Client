#version 460 core

in vert_frag
{
	vec2 tex_coord_ts;
}
IN;

layout( location = 0 ) out vec4 out_color;

uniform vec4 u_color;

uniform sampler2D u_texture;

void main()
{
	out_color = u_color * texture( u_texture, IN.tex_coord_ts );
}
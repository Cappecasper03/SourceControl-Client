#version 460 core

layout( location = 0 ) in vert_frag
{
	vec2 tex_coord_ts;
}
IN;

layout( push_constant ) uniform sPushConstant
{
	mat4 world_matrix;
	vec4 color;
}
PUSH_CONSTANT;

layout( set = 1, binding = 0 ) uniform sampler2D in_texture;

layout( location = 0 ) out vec4 out_color;

void main()
{
	out_color = PUSH_CONSTANT.color * texture( in_texture, IN.tex_coord_ts );
}
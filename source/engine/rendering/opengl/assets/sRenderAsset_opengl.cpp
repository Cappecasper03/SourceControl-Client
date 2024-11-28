#include "sRenderAsset_opengl.h"

#include <glad/glad.h>
#include <tracy/Tracy.hpp>

namespace df::opengl
{
	sRenderAsset_opengl::sRenderAsset_opengl()
	{
		ZoneScoped;

		glGenVertexArrays( 1, &vertex_array );
		glGenBuffers( 1, &vertex_buffer );
		glGenBuffers( 1, &element_buffer );
	}

	sRenderAsset_opengl::~sRenderAsset_opengl()
	{
		ZoneScoped;

		glDeleteBuffers( 1, &element_buffer );
		glDeleteBuffers( 1, &vertex_buffer );
		glDeleteVertexArrays( 1, &vertex_array );
	}
}

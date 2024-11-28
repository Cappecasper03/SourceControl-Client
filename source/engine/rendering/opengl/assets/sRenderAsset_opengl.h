#pragma once

#include "engine/misc/Misc.h"

namespace df::opengl
{
	struct sRenderAsset_opengl
	{
		DF_DISABLE_COPY_AND_MOVE( sRenderAsset_opengl );

		explicit sRenderAsset_opengl();
		virtual ~sRenderAsset_opengl();

		unsigned vertex_array;
		unsigned vertex_buffer;
		unsigned element_buffer;
	};
}

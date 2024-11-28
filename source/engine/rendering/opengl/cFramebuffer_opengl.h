#pragma once

#include <glm/vec2.hpp>
#include <string>

#include "engine/misc/Misc.h"
#include "engine/rendering/iFramebuffer.h"

namespace df::opengl
{
	class cFramebuffer_opengl : public iFramebuffer
	{
	public:
		DF_DISABLE_COPY_AND_MOVE( cFramebuffer_opengl )

		explicit cFramebuffer_opengl( std::string _name, unsigned _num_render_textures = 0, bool _generate_render_buffer = true, const glm::ivec2& _size = glm::ivec2( -1, -1 ) );
		~cFramebuffer_opengl() override;

		static void setFramebufferTexture2D( int _attachment, int _tex_target, int _texture, int _level );

		void bind() override;
		void unbind() override;

	private:
		unsigned m_buffer;
		unsigned m_render_buffer;
	};
}

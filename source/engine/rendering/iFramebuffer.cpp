#include "iFramebuffer.h"

#include <tracy/Tracy.hpp>

#include "assets/iTexture.h"

namespace df
{
	iFramebuffer::iFramebuffer( std::string _name )
		: name( std::move( _name ) )
	{}

	iFramebuffer::~iFramebuffer()
	{
		ZoneScoped;

		for( const iTexture* render_textue: render_textues )
			delete render_textue;
	}
}

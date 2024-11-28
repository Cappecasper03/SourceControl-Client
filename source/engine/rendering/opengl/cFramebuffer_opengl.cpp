#include "cFramebuffer_opengl.h"

#include <glad/glad.h>
#include <glm/vec2.hpp>
#include <tracy/Tracy.hpp>

#include "assets/cTexture_opengl.h"
#include "engine/rendering/cRenderer.h"
#include "engine/rendering/iRenderer.h"

namespace df::opengl
{
	cFramebuffer_opengl::cFramebuffer_opengl( std::string _name, const unsigned _num_render_textures, const bool _generate_render_buffer, const glm::ivec2& _size )
		: iFramebuffer( std::move( _name ) )
	{
		ZoneScoped;

		glm::ivec2 window_size = _size;
		if( window_size.x < 0 || window_size.y < 0 )
			window_size = cRenderer::getRenderInstance()->getWindowSize();

		glGenFramebuffers( 1, &m_buffer );
		cFramebuffer_opengl::bind();

		if( _generate_render_buffer )
		{
			glGenRenderbuffers( 1, &m_render_buffer );
			glBindRenderbuffer( GL_RENDERBUFFER, m_render_buffer );
			glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_STENCIL, _size.x, _size.y );
			glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_render_buffer );
		}

		std::vector< unsigned > texture_attachments{};
		for( unsigned i = 0; i < _num_render_textures; ++i )
		{
			cTexture_opengl* texture = new cTexture_opengl( "", GL_TEXTURE_2D );

			texture->bind();
			texture->setTextureParameterI( GL_TEXTURE_MIN_FILTER, GL_NEAREST );
			texture->setTextureParameterI( GL_TEXTURE_MAG_FILTER, GL_NEAREST );
			texture->setTexImage2D( 0, GL_RGBA, window_size.x, window_size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr );
			glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texture->getTexture(), 0 );

			texture_attachments.push_back( GL_COLOR_ATTACHMENT0 + i );
			render_textues.push_back( texture );
		}

		glDrawBuffers( static_cast< GLsizei >( texture_attachments.size() ), texture_attachments.data() );
		cFramebuffer_opengl::unbind();
	}

	cFramebuffer_opengl::~cFramebuffer_opengl()
	{
		ZoneScoped;

		glDeleteFramebuffers( 1, &m_buffer );
	}

	void cFramebuffer_opengl::setFramebufferTexture2D( const int _attachment, const int _tex_target, const int _texture, const int _level )
	{
		ZoneScoped;

		glFramebufferTexture2D( GL_RENDERBUFFER, _attachment, _tex_target, _texture, _level );
	}

	void cFramebuffer_opengl::bind()
	{
		ZoneScoped;

		glBindFramebuffer( GL_FRAMEBUFFER, m_buffer );
	}

	void cFramebuffer_opengl::unbind()
	{
		ZoneScoped;

		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	}
}

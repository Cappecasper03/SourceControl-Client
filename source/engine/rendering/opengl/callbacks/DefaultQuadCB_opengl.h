#pragma once

#include <glad/glad.h>
#include <tracy/TracyOpenGL.hpp>

#include "engine/managers/assets/cCameraManager.h"
#include "engine/rendering/assets/iTexture.h"
#include "engine/rendering/cRenderer.h"
#include "engine/rendering/iRenderer.h"
#include "engine/rendering/opengl/assets/cQuad_opengl.h"
#include "engine/rendering/opengl/cFramebuffer_opengl.h"
#include "engine/rendering/OpenGL/cShader_opengl.h"

namespace df::opengl::render_callback
{
	inline void defaultQuad( const cShader_opengl* _shader, const cQuad_opengl* _quad )
	{
		ZoneScoped;
		TracyGpuZone( __FUNCTION__ );

		const cCamera* camera = cCameraManager::getInstance()->current;

		_shader->use();

		_shader->setUniformMatrix4F( "u_world_matrix", _quad->transform->world );
		_shader->setUniformMatrix4F( "u_view_projection_matrix", camera->view_projection );

		_shader->setUniform4F( "u_color", _quad->color );

		_shader->setUniformSampler( "u_texture", 0 );

		if( _quad->texture )
			_quad->texture->bind();

		glEnable( GL_DEPTH_TEST );

		glBindVertexArray( _quad->vertex_array );
		glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr );

		glDisable( GL_DEPTH_TEST );
	}

	inline void defaultQuadDeferred( const cShader_opengl* _shader, const cQuad_opengl* _quad )
	{
		ZoneScoped;
		TracyGpuZone( __FUNCTION__ );

		const cFramebuffer_opengl* render_framebuffer = reinterpret_cast< const cFramebuffer_opengl* >( cRenderer::getRenderInstance()->getDeferredFramebuffer() );
		const cCamera*             camera             = cCameraManager::getInstance()->current;

		_shader->use();

		_shader->setUniformMatrix4F( "u_world_matrix", _quad->transform->world );
		_shader->setUniformMatrix4F( "u_view_projection_matrix", camera->view_projection );

		_shader->setUniformSampler( "u_position_texture", 0 );
		render_framebuffer->render_textues[ 0 ]->bind( 0 );

		_shader->setUniformSampler( "u_normal_texture", 1 );
		render_framebuffer->render_textues[ 1 ]->bind( 1 );

		_shader->setUniformSampler( "u_color_specular_texture", 2 );
		render_framebuffer->render_textues[ 2 ]->bind( 2 );

		glEnable( GL_DEPTH_TEST );

		glBindVertexArray( _quad->vertex_array );
		glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr );

		glDisable( GL_DEPTH_TEST );
	}
}

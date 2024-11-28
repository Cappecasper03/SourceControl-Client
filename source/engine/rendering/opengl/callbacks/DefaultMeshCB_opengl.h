#pragma once

#include <glad/glad.h>
#include <tracy/TracyOpenGL.hpp>

#include "engine/managers/assets/cCameraManager.h"
#include "engine/rendering/assets/iTexture.h"
#include "engine/rendering/OpenGL/assets/cMesh_opengl.h"
#include "engine/rendering/OpenGL/cShader_opengl.h"

namespace df::opengl::render_callback
{
	inline void defaultMeshAmbient( const cShader_opengl* _shader, const cMesh_opengl* _mesh )
	{
		ZoneScoped;
		TracyGpuZone( __FUNCTION__ );

		const cCamera* camera = cCameraManager::getInstance()->current;

		_shader->use();

		_shader->setUniformMatrix4F( "u_world_matrix", _mesh->transform->world );
		_shader->setUniformMatrix4F( "u_view_projection_matrix", camera->view_projection );

		_shader->setUniformSampler( "u_color_texture", 0 );
		_mesh->getTextures().at( aiTextureType_DIFFUSE )->bind();

		glEnable( GL_DEPTH_TEST );
		glEnable( GL_BLEND );

		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		glBindVertexArray( _mesh->vertex_array );
		glDrawElements( GL_TRIANGLES, static_cast< GLsizei >( _mesh->getIndices().size() ), GL_UNSIGNED_INT, nullptr );

		glDisable( GL_BLEND );
		glDisable( GL_DEPTH_TEST );
	}

	inline void defaultMesh( const cShader_opengl* _shader, const cMesh_opengl* _mesh )
	{
		ZoneScoped;

		const std::string_view name( _shader->name );

		if( name.find( "ambient" ) != std::string::npos )
			defaultMeshAmbient( _shader, _mesh );
	}

	inline void defaultMeshDeferred( const cShader_opengl* _shader, const cMesh_opengl* _mesh )
	{
		ZoneScoped;
		TracyGpuZone( __FUNCTION__ );

		const cCamera* camera = cCameraManager::getInstance()->current;

		_shader->use();

		_shader->setUniformMatrix4F( "u_world_matrix", _mesh->transform->world );
		_shader->setUniformMatrix4F( "u_view_projection_matrix", camera->view_projection );

		_shader->setUniformSampler( "u_color_texture", 0 );
		_mesh->getTextures().at( aiTextureType_DIFFUSE )->bind( 0 );

		_shader->setUniformSampler( "u_normal_texture", 1 );
		_mesh->getTextures().at( aiTextureType_NORMALS )->bind( 1 );

		_shader->setUniformSampler( "u_specular_texture", 2 );
		_mesh->getTextures().at( aiTextureType_SPECULAR )->bind( 2 );

		glEnable( GL_DEPTH_TEST );

		glBindVertexArray( _mesh->vertex_array );
		glDrawElements( GL_TRIANGLES, static_cast< GLsizei >( _mesh->getIndices().size() ), GL_UNSIGNED_INT, nullptr );

		glDisable( GL_DEPTH_TEST );
	}
}

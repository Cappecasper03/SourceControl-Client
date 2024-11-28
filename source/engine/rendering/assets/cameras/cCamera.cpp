#include "cCamera.h"

#include <GLFW/glfw3.h>
#include <glm/ext/matrix_clip_space.hpp>

#include "engine/managers/assets/cCameraManager.h"
#include "engine/managers/cEventManager.h"
#include "engine/rendering/cRenderer.h"
#include "engine/rendering/iRenderer.h"

namespace df
{
	cCamera::cCamera( std::string _name, const eType _type, const cColor& _clear_color, const float _fov, const float _near_clip, const float _far_clip )
		: iAsset( std::move( _name ) )
		, view( 1 )
		, projection( 1 )
		, view_projection( 1 )
		, clear_color( _clear_color )
		, type( _type )
		, transform( new cTransform() )
		, fov( _fov / 2 )
		, aspect_ratio( 0 )
		, near_clip( _near_clip )
		, far_clip( _far_clip )
	{
		ZoneScoped;

		cEventManager::subscribe( event::on_window_resize, this, &cCamera::onWindowResize );
	}

	cCamera::~cCamera()
	{
		ZoneScoped;

		delete transform;
	}

	void cCamera::update( const float /*_delta_time*/ )
	{
		ZoneScoped;

		transform->update();

		view = inverse( transform->world );

		view_projection = type == ePerspective ? projection * view : projection;
	}

	void cCamera::beginRender( const int _clear_buffers )
	{
		ZoneScoped;

		cCameraManager* manager = cCameraManager::getInstance();
		m_previus               = manager->current;
		manager->current        = this;

		cRenderer::getRenderInstance()->beginRendering( _clear_buffers, clear_color );
	}

	void cCamera::endRender()
	{
		ZoneScoped;

		cRenderer::getRenderInstance()->endRendering();

		cCameraManager::getInstance()->current = m_previus;
		m_previus                              = nullptr;
	}

	void cCamera::calculateProjection()
	{
		ZoneScoped;

		switch( type )
		{
			case ePerspective:
			{
				projection = glm::perspective( glm::radians( fov ), aspect_ratio, near_clip, far_clip );

				if( cRenderer::getInstanceType() & cRenderer::eInstanceType::eVulkan )
					projection[ 1 ].y *= -1;
			}
			break;
			case eOrthographic:
			{
				projection = glm::ortho( 0.f, ortographic_size.x, 0.f, ortographic_size.y, near_clip, far_clip );

				if( cRenderer::getInstanceType() & cRenderer::eInstanceType::eVulkan )
				{
					constexpr glm::mat4 correction( glm::vec4( 1.0f, 0.0f, 0.0f, 0.0f ),
					                                glm::vec4( 0.0f, -1.0f, 0.0f, 0.0f ),
					                                glm::vec4( 0.0f, 0.0f, 0.5f, 0.0f ),
					                                glm::vec4( 0.0f, 0.0f, 0.5f, 1.0f ) );

					projection = correction * projection;
				}
			}
		}
	}

	void cCamera::onWindowResize( const int _width, const int _height )
	{
		ZoneScoped;

		aspect_ratio       = static_cast< float >( _width ) / static_cast< float >( _height );
		ortographic_size.x = static_cast< float >( _width );
		ortographic_size.y = static_cast< float >( _height );
		calculateProjection();
		update();
	}
}

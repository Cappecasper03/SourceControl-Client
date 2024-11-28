#include "iRenderer.h"

#include <GLFW/glfw3.h>
#include <tracy/Tracy.hpp>

#include "engine/managers/cEventManager.h"

namespace df
{
	void iRenderer::resizeWindow( const int _width, const int _height ) const
	{
		ZoneScoped;

		if( _width > 0 && _height > 0 )
		{
			glfwSetWindowSize( m_window, _width, _height );
			return;
		}

		cEventManager::invoke( event::on_window_resize, m_window_size.x, m_window_size.y );
	}

	void iRenderer::setCursorInputMode( const int _mode ) const
	{
		ZoneScoped;

		glfwSetInputMode( m_window, GLFW_CURSOR, _mode );
	}
}

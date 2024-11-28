#include "cInputManager.h"

#include <GLFW/glfw3.h>

#include "cEventManager.h"
#include "engine/rendering/cRenderer.h"
#include "engine/rendering/iRenderer.h"

namespace df
{
	cInputManager::cInputManager()
	{
		ZoneScoped;

		if( cRenderer::getInstanceType() & ( cRenderer::eInstanceType::eOpenGL | cRenderer::eInstanceType::eVulkan ) )
		{
			GLFWwindow* window = cRenderer::getRenderInstance()->getWindow();

			glfwSetKeyCallback( window, keyCallback );
			glfwSetMouseButtonCallback( window, mouseButtonCallback );
			glfwSetCursorPosCallback( window, cursorPositionCallback );
			glfwSetScrollCallback( window, scrollCallback );
			glfwSetCursorEnterCallback( window, cursorEnterCallback );
		}
	}

	cInputManager::~cInputManager()
	{
		if( cRenderer::getInstanceType() & ( cRenderer::eInstanceType::eOpenGL | cRenderer::eInstanceType::eVulkan ) )
		{
			GLFWwindow* window = cRenderer::getRenderInstance()->getWindow();

			glfwSetKeyCallback( window, nullptr );
			glfwSetMouseButtonCallback( window, nullptr );
			glfwSetCursorPosCallback( window, nullptr );
			glfwSetScrollCallback( window, nullptr );
			glfwSetCursorEnterCallback( window, nullptr );
		}
	}

	void cInputManager::update()
	{
		ZoneScoped;

		input::sInput& input = getInstance()->m_input;

		glfwPollEvents();

		if( input.keyboard.empty() && input.mouse_button.empty() && !input.mouse_cursor.updated && !input.mouse_scroll.updated )
			return;

		const input::sInput& const_input = input;
		cEventManager::invoke( event::input, const_input );

		input.keyboard.clear();
		input.mouse_button.clear();
		input.mouse_cursor.updated = false;
		input.mouse_scroll.updated = false;
	}

	bool cInputManager::checkKey( const int _key, const int _action )
	{
		ZoneScoped;

		std::unordered_map< int, input::sKeyboard >& keyboard = getInstance()->m_input.keyboard;
		const auto                                   key_it   = keyboard.find( _key );

		if( key_it != keyboard.end() && key_it->second.action == _action )
			return true;

		return false;
	}

	input::eAction cInputManager::checkKey( const int _key )
	{
		ZoneScoped;

		std::unordered_map< int, input::sKeyboard >& keyboard = getInstance()->m_input.keyboard;
		const auto                                   key_it   = keyboard.find( _key );

		if( key_it != keyboard.end() )
		{
			switch( key_it->second.action )
			{
				case GLFW_PRESS:
					return input::ePress;
				case GLFW_RELEASE:
					return input::eRelease;
				case GLFW_REPEAT:
					return input::eRepeat;
				default:
					return input::eNone;
			}
		}

		return input::eNone;
	}

	bool cInputManager::checkButton( const int _key, const int _action )
	{
		ZoneScoped;

		std::unordered_map< int, input::sMouseButton >& mouse_button = getInstance()->m_input.mouse_button;
		const auto                                      button_it    = mouse_button.find( _key );

		if( button_it != mouse_button.end() && button_it->second.action == _action )
			return true;

		return false;
	}

	input::eAction cInputManager::checkButton( const int _key )
	{
		ZoneScoped;

		std::unordered_map< int, input::sMouseButton >& mouse_button = getInstance()->m_input.mouse_button;
		const auto                                      button_it    = mouse_button.find( _key );

		if( button_it != mouse_button.end() )
		{
			switch( button_it->second.action )
			{
				case GLFW_PRESS:
					return input::ePress;
				case GLFW_RELEASE:
					return input::eRelease;
				case GLFW_REPEAT:
					return input::eRepeat;
				default:
					return input::eNone;
			}
		}

		return input::eNone;
	}

	void cInputManager::keyCallback( GLFWwindow* /*_window*/, const int _key, const int _scancode, const int _action, const int _mods )
	{
		ZoneScoped;

		getInstance()->m_input.keyboard[ _key ] = { _scancode, _action, _mods };
	}

	void cInputManager::mouseButtonCallback( GLFWwindow* /*_window*/, const int _button, const int _action, const int _mods )
	{
		ZoneScoped;

		getInstance()->m_input.mouse_button[ _button ] = { _action, _mods };
	}

	void cInputManager::cursorPositionCallback( GLFWwindow* /*_window*/, const double _x_position, const double _y_position )
	{
		ZoneScoped;

		input::sMouseCursor& mouse_cursor = getInstance()->m_input.mouse_cursor;

		mouse_cursor.updated = true;

		if( mouse_cursor.on_window_current && mouse_cursor.on_window_previus )
		{
			mouse_cursor.x_delta = mouse_cursor.x_current - mouse_cursor.x_previus;
			mouse_cursor.y_delta = mouse_cursor.y_current - mouse_cursor.y_previus;

			mouse_cursor.x_previus = mouse_cursor.x_current;
			mouse_cursor.y_previus = mouse_cursor.y_current;
		}
		else
		{
			mouse_cursor.x_previus = _x_position;
			mouse_cursor.y_previus = _y_position;
		}

		mouse_cursor.x_current = _x_position;
		mouse_cursor.y_current = _y_position;

		mouse_cursor.on_window_previus = mouse_cursor.on_window_current;
	}

	void cInputManager::scrollCallback( GLFWwindow* /*_window*/, const double _x_offset, const double _y_offset )
	{
		ZoneScoped;

		input::sMouseScroll& mouse_scroll = getInstance()->m_input.mouse_scroll;

		mouse_scroll.updated = true;

		mouse_scroll.x_offset = _x_offset;
		mouse_scroll.y_offset = _y_offset;
	}

	void cInputManager::cursorEnterCallback( GLFWwindow* /*_window*/, const int _entered )
	{
		ZoneScoped;

		input::sMouseCursor& mouse_cursor = getInstance()->m_input.mouse_cursor;

		mouse_cursor.on_window_previus = mouse_cursor.on_window_current;
		mouse_cursor.on_window_current = _entered;
	}
}

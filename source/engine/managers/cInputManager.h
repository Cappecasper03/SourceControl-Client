#pragma once

#include "engine/input/Input.h"
#include "engine/misc/iSingleton.h"

struct GLFWwindow;

namespace df
{
	class cInputManager final : public iSingleton< cInputManager >
	{
	public:
		DF_DISABLE_COPY_AND_MOVE( cInputManager );

		cInputManager();
		~cInputManager() override;

		static void update();

		static bool           checkKey( int _key, int _action );
		static input::eAction checkKey( int _key );
		static bool           checkButton( int _key, int _action );
		static input::eAction checkButton( int _key );

	private:
		static void keyCallback( GLFWwindow* _window, int _key, int _scancode, int _action, int _mods );
		static void mouseButtonCallback( GLFWwindow* _window, int _button, int _action, int _mods );
		static void cursorPositionCallback( GLFWwindow* _window, double _x_position, double _y_position );
		static void scrollCallback( GLFWwindow* _window, double _x_offset, double _y_offset );
		static void cursorEnterCallback( GLFWwindow* _window, int _entered );

		input::sInput m_input;
	};
}

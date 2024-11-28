#pragma once

#include <unordered_map>

namespace df::input
{
	enum eAction
	{
		ePress   = 1,
		eRelease = -1,
		eRepeat  = 2,
		eNone    = 0,
	};

	struct sKeyboard
	{
		int scancode = -1;
		int action   = -1;
		int mods     = -1;
	};

	struct sMouseButton
	{
		int action = -1;
		int mods   = -1;
	};

	struct sMouseCursor
	{
		bool updated = false;

		int on_window_current = 0;
		int on_window_previus = 0;

		double x_delta = 0;
		double y_delta = 0;

		double x_previus = -1;
		double y_previus = -1;

		double x_current = -1;
		double y_current = -1;
	};

	struct sMouseScroll
	{
		bool updated = false;

		double x_offset = 0;
		double y_offset = 0;
	};

	struct sInput
	{
		std::unordered_map< int, sKeyboard >    keyboard;
		std::unordered_map< int, sMouseButton > mouse_button;
		sMouseCursor                            mouse_cursor = {};
		sMouseScroll                            mouse_scroll = {};
	};
}

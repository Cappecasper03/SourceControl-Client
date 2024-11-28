#pragma once

#include <string>

#include "engine/misc/Misc.h"

namespace df
{
	class iShader
	{
	public:
		DF_DISABLE_COPY_AND_MOVE( iShader );

		explicit iShader( std::string _name )
			: name( std::move( _name ) )
		{}

		virtual ~iShader() = default;

		const std::string name;
	};
}

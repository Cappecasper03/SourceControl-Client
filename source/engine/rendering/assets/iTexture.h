#pragma once

#include <string>

#include "engine/misc/Misc.h"

namespace df
{
	class iTexture
	{
	public:
		DF_DISABLE_COPY_AND_MOVE( iTexture );

		explicit iTexture( std::string _name );
		virtual ~iTexture() = default;

		virtual bool load( const std::string& _file_path, bool _mipmapped = false, int _mipmaps = 0, bool _flip_vertically_on_load = true ) = 0;

		virtual void bind( int /*_index*/ = 0 )   = 0;
		virtual void unbind( int /*_index*/ = 0 ) = 0;

		std::string name;

	protected:
		std::string m_file_path;
	};
}

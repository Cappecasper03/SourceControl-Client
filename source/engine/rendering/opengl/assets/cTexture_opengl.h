#pragma once

#include <string>

#include "engine/misc/Misc.h"
#include "engine/rendering/assets/iTexture.h"

namespace df::opengl
{
	class cTexture_opengl : public iTexture
	{
	public:
		DF_DISABLE_COPY_AND_MOVE( cTexture_opengl );

		explicit cTexture_opengl( std::string _name, int _target );
		~cTexture_opengl() override;

		bool load( const std::string& _file, bool _mipmapped = false, int _mipmaps = 0, bool _flip_vertically_on_load = true ) override;

		void setTexImage2D( int _level, int _internal_format, int _width, int _height, int _border, unsigned _format, unsigned _type, const void* _pixels ) const;
		void setTextureParameterI( int _name, int _param ) const;
		void setPixelStoreI( int _name, int _param ) const;

		void bind( int _index = 0 ) override;
		void unbind( int _index = 0 ) override;

		unsigned getTexture() const { return m_texture; }

	protected:
		unsigned m_texture;
		int      m_target;
	};
}

#include "AssetTypes.h"

#include <tracy/Tracy.hpp>

namespace df
{
	iAsset::iAsset( std::string _name )
		: name( std::move( _name ) )
	{}

	iRenderAsset::iRenderAsset( std::string _name )
		: iAsset( std::move( _name ) )
		, transform( new cTransform() )
		, render_callback( nullptr )
	{}

	iRenderAsset::~iRenderAsset()
	{
		ZoneScoped;

		delete transform;
	}
}

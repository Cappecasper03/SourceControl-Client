#include "cLight.h"

namespace df
{
	cLight::cLight( std::string _name )
		: iAsset( std::move( _name ) )
		, type( eAmbient )
		, transform( new cTransform() )
		, intensity( 1 )
		, radius( 0 )
		, cone_angle( 0 )
		, color( color::white )
	{}

	cLight::cLight( std::string _name, const eType _type, const float _intensity, const float _radius, const float _cone_angle, const cColor& _color )
		: iAsset( std::move( _name ) )
		, type( _type )
		, transform( new cTransform() )
		, intensity( _intensity )
		, radius( _radius )
		, cone_angle( _cone_angle )
		, color( _color )
	{}
}

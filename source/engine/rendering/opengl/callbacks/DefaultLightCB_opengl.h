#pragma once

#include "engine/managers/assets/cCameraManager.h"
#include "engine/rendering/assets/cLight.h"
#include "engine/rendering/OpenGL/cShader_opengl.h"

namespace df::opengl::render_callback
{
	inline void defaultLight( const cShader_opengl* _shader, const cLight* _light )
	{
		ZoneScoped;

		switch( _light->type )
		{
			case cLight::eAmbient:
				break;
			case cLight::eDirectional:
				break;
			case cLight::ePoint:
				break;
			case cLight::eSpot:
				break;
		}
	}
}

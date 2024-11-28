#include "cLightManager.h"

namespace df
{
	cLightManager::cLightManager()
	{
		ZoneScoped;

		create( "ambient", cLight::eType::eAmbient, 1.f );
	}
}

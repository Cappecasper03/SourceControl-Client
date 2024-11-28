#include "cCameraManager.h"

#include "engine/rendering/assets/cameras/cCamera.h"
#include "engine/rendering/assets/cameras/cFreeFlightCamera.h"

namespace df
{
	cCameraManager::cCameraManager()
	{
		ZoneScoped;

		current = create( "default_3d", cCamera::eType::ePerspective, cColor( .5f, .75f, 1, 1 ), 90.f );
		create( "default_2d", cCamera::eType::eOrthographic, color::white, 90.f, -1.f, 100.f );
	}
}

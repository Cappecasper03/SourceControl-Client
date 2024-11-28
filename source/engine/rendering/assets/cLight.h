#pragma once

#include "AssetTypes.h"
#include "engine/misc/cColor.h"

namespace df
{
	class cLight : public iAsset
	{
	public:
		DF_DISABLE_COPY_AND_MOVE( cLight )

		enum eType
		{
			eAmbient,
			eDirectional,
			ePoint,
			eSpot,
		};

		explicit cLight( std::string _name );
		explicit cLight( std::string _name, eType _type, float _intensity = 1, float _radius = 0, float _cone_angle = 0, const cColor& _color = color::white );
		~cLight() override = default;

		eType       type;
		cTransform* transform;
		float       intensity;
		float       radius;
		float       cone_angle;
		cColor      color;
	};
}

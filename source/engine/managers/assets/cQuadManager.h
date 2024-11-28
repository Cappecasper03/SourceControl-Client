#pragma once

#include "engine/rendering/assets/iQuad.h"
#include "iAssetManager.h"

namespace df
{
	class cQuadManager final : public iAssetManager< cQuadManager, iQuad >
	{
	public:
		DF_DISABLE_COPY_AND_MOVE( cQuadManager )

		cQuadManager();
		~cQuadManager() override;

		static iQuad* load( const std::string& _name, const glm::vec3& _position, const glm::vec2& _size, const cColor& _color = color::white );
	};
}

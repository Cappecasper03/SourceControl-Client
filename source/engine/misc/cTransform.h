#pragma once

#include <glm/mat4x4.hpp>
#include <vector>

#include "engine/misc/Misc.h"

namespace df
{
	class cTransform final
	{
	public:
		DF_DISABLE_COPY( cTransform )

		cTransform();
		cTransform( cTransform&& _other ) = default;
		~cTransform();

		cTransform& operator=( cTransform&& _other ) = default;

		void update();

		bool addChild( cTransform& _child );
		bool removeChild( cTransform& _child );

		bool setParent( cTransform& _parent );
		bool removeParent();

		glm::mat4 local;
		glm::mat4 world;

		cTransform*                parent;
		std::vector< cTransform* > children;
	};
}

#pragma once

#include <glm/vec3.hpp>
#include <string>

#include "engine/misc/cColor.h"
#include "engine/rendering/assets/iQuad.h"
#include "sRenderAsset_opengl.h"

namespace df::opengl
{
	class cTexture_opengl;

	class cQuad_opengl : public sRenderAsset_opengl,
						 public iQuad
	{
	public:
		DF_DISABLE_COPY_AND_MOVE( cQuad_opengl );

		cQuad_opengl( std::string _name, const glm::vec3& _position, const glm::vec2& _size, const cColor& _color = color::white );
		~cQuad_opengl() override = default;

		bool loadTexture( const std::string& _file_path, bool _mipmapped, int _mipmaps, bool _flip_vertically_on_load ) override;

		void render() override;

		static iRenderCallback* createDefaults();

	protected:
		struct sVertex
		{
			glm::vec3 position;
			glm::vec2 tex_coord;
		};
	};
}

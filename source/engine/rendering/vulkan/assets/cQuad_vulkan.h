#pragma once

#include <glm/vec3.hpp>
#include <string>

#include "engine/misc/cColor.h"
#include "engine/rendering/assets/iQuad.h"
#include "sRenderAsset_vulkan.h"

namespace df
{
	class cQuadManager;
}

namespace df::vulkan
{
	class cTexture_vulkan;

	class cQuad_vulkan : public sRenderAsset_vulkan,
						 public iQuad
	{
	public:
		DF_DISABLE_COPY_AND_MOVE( cQuad_vulkan );

		struct sPushConstants
		{
			glm::mat4 world_matrix;
			cColor    color;
		};

		cQuad_vulkan( std::string _name, const glm::vec3& _position, const glm::vec2& _size, const cColor& _color = color::white );
		~cQuad_vulkan() override = default;

		bool loadTexture( const std::string& _file_path, bool _mipmapped, int _mipmaps, bool _flip_vertically_on_load ) override;

		void render() override;

		static iRenderCallback* createDefaults();
		static void             destroyDefaults();

		vk::DescriptorSetLayout getTextureLayout() const { return s_texture_layout.get(); }

	private:
		static vk::UniqueDescriptorSetLayout s_texture_layout;
	};
}

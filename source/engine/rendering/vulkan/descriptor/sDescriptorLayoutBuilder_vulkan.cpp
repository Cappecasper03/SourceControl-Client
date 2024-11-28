#include "sDescriptorLayoutBuilder_vulkan.h"

#include <tracy/Tracy.hpp>

#include "engine/rendering/cRenderer.h"
#include "engine/rendering/vulkan/cRenderer_vulkan.h"

namespace df::vulkan
{

	vk::UniqueDescriptorSetLayout sDescriptorLayoutBuilder_vulkan::build( const vk::ShaderStageFlags _shader_stages )
	{
		ZoneScoped;

		const cRenderer_vulkan* renderer = reinterpret_cast< cRenderer_vulkan* >( cRenderer::getRenderInstance() );
		return build( renderer->getLogicalDevice(), _shader_stages );
	}

	vk::UniqueDescriptorSetLayout sDescriptorLayoutBuilder_vulkan::build( const vk::Device& _logical_device, const vk::ShaderStageFlags _shader_stages )
	{
		ZoneScoped;

		for( vk::DescriptorSetLayoutBinding& binding: bindings )
			binding.stageFlags |= _shader_stages;

		const vk::DescriptorSetLayoutCreateInfo create_info( vk::DescriptorSetLayoutCreateFlags(), static_cast< uint32_t >( bindings.size() ), bindings.data() );

		return _logical_device.createDescriptorSetLayoutUnique( create_info ).value;
	}
}

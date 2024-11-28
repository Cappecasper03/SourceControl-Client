#pragma once

#include <deque>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace df::vulkan
{

	struct sDescriptorWriter_vulkan
	{
		void writeImage( uint32_t _binding, const vk::ImageView& _image, const vk::Sampler& _sampler, vk::ImageLayout _layout, vk::DescriptorType _type );
		void writeBuffer( uint32_t _binding, const vk::Buffer& _buffer, size_t _size, size_t _offset, vk::DescriptorType _type );

		void clear();
		void updateSet( vk::DescriptorSet _set );

		std::deque< vk::DescriptorImageInfo >  image_infos;
		std::deque< vk::DescriptorBufferInfo > buffer_infos;
		std::vector< vk::WriteDescriptorSet >  writes;
	};
}

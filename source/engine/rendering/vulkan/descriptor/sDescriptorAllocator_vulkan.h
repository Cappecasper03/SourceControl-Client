#pragma once

#include <span>
#include <vector>
#include <vulkan/vulkan_shared.hpp>

namespace df::vulkan
{

	struct sDescriptorAllocator_vulkan
	{
		struct sPoolSizeRatio
		{
			vk::DescriptorType type;
			float              ratio;
		};

		sDescriptorAllocator_vulkan();

		void create( const vk::Device& _logical_device, uint32_t _initial_sets, const std::span< sPoolSizeRatio >& _pool_ratios );
		void destroy();
		void clear();

		vk::DescriptorSet& allocate( const vk::DescriptorSetLayout& _layout );

	protected:
		vk::DescriptorPool       getPool();
		vk::UniqueDescriptorPool createPool( uint32_t _set_count, const std::span< sPoolSizeRatio >& _pool_ratios ) const;

		std::vector< sPoolSizeRatio >     m_ratios;
		std::vector< vk::DescriptorPool > m_full_pools;
		std::vector< vk::DescriptorPool > m_ready_pools;
		uint32_t                          m_sets_per_pool;

	private:
		std::vector< vk::UniqueDescriptorPool > m_pools;
		std::vector< vk::UniqueDescriptorSet >  m_sets;

		vk::Device m_logical_device;
	};
}

#pragma once

#include <string>
#include <vk_mem_alloc.hpp>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_core.h>

#include "Types_vulkan.h"

namespace df::vulkan::helper
{
	namespace init
	{
		vk::CommandPoolCreateInfo     commandPoolCreateInfo( uint32_t _queue_family_index, vk::CommandPoolCreateFlags _flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer );
		vk::CommandBufferAllocateInfo commandBufferAllocateInfo( const vk::CommandPool& _command_pool, uint32_t _count = 1 );
		vk::CommandBufferBeginInfo    commandBufferBeginInfo( vk::CommandBufferUsageFlags _flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit );

		vk::FenceCreateInfo     fenceCreateInfo( vk::FenceCreateFlags _flags = vk::FenceCreateFlagBits::eSignaled );
		vk::SemaphoreCreateInfo semaphoreCreateInfo();

		vk::SemaphoreSubmitInfo     semaphoreSubmitInfo( vk::PipelineStageFlags2 _stage_mask, const vk::Semaphore& _semaphore );
		vk::CommandBufferSubmitInfo commandBufferSubmitInfo( const vk::CommandBuffer& _command_buffer );

		vk::SubmitInfo2 submitInfo( const vk::CommandBufferSubmitInfo* _command_buffer        = nullptr,
		                            const vk::SemaphoreSubmitInfo*     _signal_semaphore_info = nullptr,
		                            const vk::SemaphoreSubmitInfo*     _wait_semaphore_info   = nullptr );
		vk::SubmitInfo2 submitInfo( const std::vector< vk::CommandBufferSubmitInfo >& _command_buffers        = {},
		                            const std::vector< vk::SemaphoreSubmitInfo >&     _signal_semaphore_infos = {},
		                            const std::vector< vk::SemaphoreSubmitInfo >&     _wait_semaphore_infos   = {} );

		vk::PresentInfoKHR presentInfo( const vk::Semaphore* _semaphore = nullptr, const vk::SwapchainKHR* _swapchain = nullptr, const uint32_t* _swap_chain_index = nullptr );
		vk::PresentInfoKHR presentInfo( const std::vector< vk::Semaphore >&    _semaphores         = {},
		                                const std::vector< vk::SwapchainKHR >& _swapchains         = {},
		                                const std::vector< uint32_t >&         _swap_chain_indices = {} );

		vk::RenderingAttachmentInfo attachmentInfo( const vk::ImageView& _view, const vk::ClearValue* _clear, vk::ImageLayout _layout = vk::ImageLayout::eColorAttachmentOptimal );
		vk::RenderingAttachmentInfo depthAttachmentInfo( const vk::ImageView& _view, vk::ImageLayout _layout = vk::ImageLayout::eColorAttachmentOptimal );

		vk::ImageCreateInfo       imageCreateInfo( vk::Format _format, vk::ImageUsageFlags _usage_flags, vk::Extent3D _extent );
		vk::ImageViewCreateInfo   imageViewCreateInfo( vk::Format _format, const vk::Image& _image, vk::ImageAspectFlags _aspect_flags );
		vk::ImageSubresourceRange imageSubresourceRange( vk::ImageAspectFlags _aspect_mask );

		vk::PipelineShaderStageCreateInfo pipelineShaderStageCreateInfo( vk::ShaderStageFlagBits _stage, const vk::ShaderModule& _module );

		vk::RenderingInfo renderingInfo( vk::Extent2D                       _extent,
		                                 const vk::RenderingAttachmentInfo* _color_attachment = nullptr,
		                                 const vk::RenderingAttachmentInfo* _depth_attachment = nullptr );
		vk::RenderingInfo renderingInfo( vk::Extent2D                                      _extent,
		                                 const std::vector< vk::RenderingAttachmentInfo >& _color_attachments = {},
		                                 const vk::RenderingAttachmentInfo*                _depth_attachment  = nullptr );
	}

	namespace util
	{
		void transitionImage( const vk::CommandBuffer& _command_buffer, const vk::Image& _image, vk::ImageLayout _current_layout, vk::ImageLayout _new_layout );
		void copyImageToImage( const vk::CommandBuffer& _command_buffer,
		                       const vk::Image&         _source,
		                       const vk::Image&         _destination,
		                       vk::Extent2D             _source_size,
		                       vk::Extent2D             _destination_size );

		vk::ShaderModule createShaderModule( const std::string& _name );

		void                    createBuffer( vk::DeviceSize _size, vk::BufferUsageFlags _usage_flags, vma::MemoryUsage _memory_usage, sAllocatedBuffer_vulkan& _buffer );
		void                    createBuffer( vk::DeviceSize           _size,
		                                      vk::BufferUsageFlags     _usage_flags,
		                                      vma::MemoryUsage         _memory_usage,
		                                      sAllocatedBuffer_vulkan& _buffer,
		                                      const vma::Allocator&    _memory_allocator );
		sAllocatedBuffer_vulkan createBuffer( vk::DeviceSize _size, vk::BufferUsageFlags _usage_flags, vma::MemoryUsage _memory_usage );
		sAllocatedBuffer_vulkan createBuffer( vk::DeviceSize _size, vk::BufferUsageFlags _usage_flags, vma::MemoryUsage _memory_usage, const vma::Allocator& _memory_allocator );
		void                    destroyBuffer( sAllocatedBuffer_vulkan& _buffer );
		void                    destroyBuffer( sAllocatedBuffer_vulkan& _buffer, vma::Allocator& _memory_allocator );

		sAllocatedImage_vulkan createImage( vk::Extent3D _size, vk::Format _format, vk::ImageUsageFlags _usage, bool _mipmapped = false, unsigned _mipmaps = 0 );
		sAllocatedImage_vulkan createImage( const void* _data, vk::Extent3D _size, vk::Format _format, vk::ImageUsageFlags _usage, bool _mipmapped = false, unsigned _mipmaps = 0 );
		void                   destroyImage( sAllocatedImage_vulkan& _image );
	}
}

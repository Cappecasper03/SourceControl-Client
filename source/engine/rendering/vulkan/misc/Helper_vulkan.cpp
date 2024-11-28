#include "Helper_vulkan.h"

#include <fmt/format.h>
#include <tracy/Tracy.hpp>
#include <vector>
#include <vk_mem_alloc.hpp>

#include "engine/filesystem/cFileSystem.h"
#include "engine/log/Log.h"
#include "engine/rendering/cRenderer.h"
#include "engine/rendering/vulkan/cRenderer_vulkan.h"

namespace df::vulkan::helper
{
	namespace init
	{
		vk::CommandPoolCreateInfo commandPoolCreateInfo( const uint32_t _queue_family_index, const vk::CommandPoolCreateFlags _flags )
		{
			ZoneScoped;

			const vk::CommandPoolCreateInfo create_info( _flags, _queue_family_index );
			return create_info;
		}

		vk::CommandBufferAllocateInfo commandBufferAllocateInfo( const vk::CommandPool& _command_pool, const uint32_t _count )
		{
			ZoneScoped;

			const vk::CommandBufferAllocateInfo allocate_info( _command_pool, vk::CommandBufferLevel::ePrimary, _count );
			return allocate_info;
		}

		vk::CommandBufferBeginInfo commandBufferBeginInfo( const vk::CommandBufferUsageFlags _flags )
		{
			ZoneScoped;

			const vk::CommandBufferBeginInfo begin_info( _flags );
			return begin_info;
		}

		vk::FenceCreateInfo fenceCreateInfo( const vk::FenceCreateFlags _flags )
		{
			ZoneScoped;

			const vk::FenceCreateInfo create_info( _flags );
			return create_info;
		}
		vk::SemaphoreCreateInfo semaphoreCreateInfo()
		{
			ZoneScoped;

			return vk::SemaphoreCreateInfo();
		}

		vk::SemaphoreSubmitInfo semaphoreSubmitInfo( const vk::PipelineStageFlags2 _stage_mask, const vk::Semaphore& _semaphore )
		{
			ZoneScoped;

			const vk::SemaphoreSubmitInfo submit_info( _semaphore, static_cast< uint32_t >( 1 ), _stage_mask, 0 );
			return submit_info;
		}

		vk::CommandBufferSubmitInfo commandBufferSubmitInfo( const vk::CommandBuffer& _command_buffer )
		{
			ZoneScoped;

			const vk::CommandBufferSubmitInfo submit_info( _command_buffer, 0 );
			return submit_info;
		}

		vk::SubmitInfo2 submitInfo( const vk::CommandBufferSubmitInfo* _command_buffer,
		                            const vk::SemaphoreSubmitInfo*     _signal_semaphore_info,
		                            const vk::SemaphoreSubmitInfo*     _wait_semaphore_info )
		{
			ZoneScoped;

			const uint32_t wait_count    = _wait_semaphore_info ? 1 : 0;
			const uint32_t command_count = _command_buffer ? 1 : 0;
			const uint32_t signal_count  = _signal_semaphore_info ? 1 : 0;

			const vk::SubmitInfo2 submit_info( vk::SubmitFlags(), wait_count, _wait_semaphore_info, command_count, _command_buffer, signal_count, _signal_semaphore_info );
			return submit_info;
		}

		vk::SubmitInfo2 submitInfo( const std::vector< vk::CommandBufferSubmitInfo >& _command_buffers,
		                            const std::vector< vk::SemaphoreSubmitInfo >&     _signal_semaphore_infos,
		                            const std::vector< vk::SemaphoreSubmitInfo >&     _wait_semaphore_infos )
		{
			ZoneScoped;

			const vk::SubmitInfo2 submit_info( vk::SubmitFlags(), _wait_semaphore_infos, _command_buffers, _signal_semaphore_infos );
			return submit_info;
		}

		vk::PresentInfoKHR presentInfo( const vk::Semaphore* _semaphore, const vk::SwapchainKHR* _swapchain, const uint32_t* _swap_chain_index )
		{
			ZoneScoped;

			const uint32_t semaphore_count = _semaphore ? 1 : 0;
			const uint32_t swapchain_count = _swapchain ? 1 : 0;

			const vk::PresentInfoKHR present_info( semaphore_count, _semaphore, swapchain_count, _swapchain, _swap_chain_index );
			return present_info;
		}

		vk::PresentInfoKHR presentInfo( const std::vector< vk::Semaphore >&    _semaphores,
		                                const std::vector< vk::SwapchainKHR >& _swapchains,
		                                const std::vector< uint32_t >&         _swap_chain_indices )
		{
			ZoneScoped;

			const vk::PresentInfoKHR present_info( _semaphores, _swapchains, _swap_chain_indices );
			return present_info;
		}

		vk::RenderingAttachmentInfo attachmentInfo( const vk::ImageView& _view, const vk::ClearValue* _clear, const vk::ImageLayout _layout )
		{
			ZoneScoped;

			const vk::RenderingAttachmentInfo attachment_info( _view,
			                                                   _layout,
			                                                   vk::ResolveModeFlagBits::eNone,
			                                                   nullptr,
			                                                   vk::ImageLayout::eUndefined,
			                                                   _clear ? vk::AttachmentLoadOp::eClear : vk::AttachmentLoadOp::eLoad,
			                                                   vk::AttachmentStoreOp::eStore,
			                                                   _clear ? *_clear : vk::ClearValue{} );
			return attachment_info;
		}

		vk::RenderingAttachmentInfo depthAttachmentInfo( const vk::ImageView& _view, const vk::ImageLayout _layout )
		{
			ZoneScoped;

			const vk::RenderingAttachmentInfo attachment_info( _view,
			                                                   _layout,
			                                                   vk::ResolveModeFlagBits::eNone,
			                                                   nullptr,
			                                                   vk::ImageLayout::eUndefined,
			                                                   vk::AttachmentLoadOp::eClear,
			                                                   vk::AttachmentStoreOp::eStore,
			                                                   vk::ClearValue( vk::ClearDepthStencilValue( 0 ) ) );
			return attachment_info;
		}

		vk::ImageCreateInfo imageCreateInfo( const vk::Format _format, const vk::ImageUsageFlags _usage_flags, const vk::Extent3D _extent )
		{
			ZoneScoped;

			const vk::ImageCreateInfo
				create_info( vk::ImageCreateFlags(), vk::ImageType::e2D, _format, _extent, 1, 1, vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal, _usage_flags );
			return create_info;
		}

		vk::ImageViewCreateInfo imageViewCreateInfo( const vk::Format _format, const vk::Image& _image, const vk::ImageAspectFlags _aspect_flags )
		{
			ZoneScoped;

			const vk::ImageViewCreateInfo info( vk::ImageViewCreateFlags(),
			                                    _image,
			                                    vk::ImageViewType::e2D,
			                                    _format,
			                                    vk::ComponentMapping(),
			                                    vk::ImageSubresourceRange( _aspect_flags, 0, 1, 0, 1 ) );
			return info;
		}

		vk::ImageSubresourceRange imageSubresourceRange( const vk::ImageAspectFlags _aspect_mask )
		{
			ZoneScoped;

			const vk::ImageSubresourceRange subresource_range( _aspect_mask, 0, vk::RemainingMipLevels, 0, vk::RemainingArrayLayers );
			return subresource_range;
		}

		vk::PipelineShaderStageCreateInfo pipelineShaderStageCreateInfo( const vk::ShaderStageFlagBits _stage, const vk::ShaderModule& _module )
		{
			ZoneScoped;

			const vk::PipelineShaderStageCreateInfo info( vk::PipelineShaderStageCreateFlags(), _stage, _module, "main" );
			return info;
		}

		vk::RenderingInfo renderingInfo( const vk::Extent2D _extent, const vk::RenderingAttachmentInfo* _color_attachment, const vk::RenderingAttachmentInfo* _depth_attachment )
		{
			ZoneScoped;

			const vk::RenderingInfo info( vk::RenderingFlags(), vk::Rect2D( vk::Offset2D(), _extent ), 1, 0, 1, _color_attachment, _depth_attachment );
			return info;
		}

		vk::RenderingInfo renderingInfo( const vk::Extent2D                                _extent,
		                                 const std::vector< vk::RenderingAttachmentInfo >& _color_attachments,
		                                 const vk::RenderingAttachmentInfo*                _depth_attachment )
		{
			ZoneScoped;

			const vk::RenderingInfo info( vk::RenderingFlags(), vk::Rect2D( vk::Offset2D(), _extent ), 1, 0, _color_attachments, _depth_attachment );
			return info;
		}
	}

	namespace util
	{
		void transitionImage( const vk::CommandBuffer& _command_buffer, const vk::Image& _image, const vk::ImageLayout _current_layout, const vk::ImageLayout _new_layout )
		{
			ZoneScoped;

			const vk::ImageMemoryBarrier2 memory_barrier(
				vk::PipelineStageFlagBits2::eAllCommands,
				vk::AccessFlagBits2::eMemoryWrite,
				vk::PipelineStageFlagBits2::eAllCommands,
				vk::AccessFlagBits2::eMemoryWrite | vk::AccessFlagBits2::eMemoryRead,
				_current_layout,
				_new_layout,
				0,
				0,
				_image,
				init::imageSubresourceRange( _new_layout == vk::ImageLayout::eDepthAttachmentOptimal ? vk::ImageAspectFlagBits::eDepth : vk::ImageAspectFlagBits::eColor ) );

			const vk::DependencyInfo info( vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, &memory_barrier );
			_command_buffer.pipelineBarrier2( info );
		}

		void copyImageToImage( const vk::CommandBuffer& _command_buffer,
		                       const vk::Image&         _source,
		                       const vk::Image&         _destination,
		                       const vk::Extent2D       _source_size,
		                       const vk::Extent2D       _destination_size )
		{
			ZoneScoped;

			const std::array< vk::Offset3D, 2 > source{
				{
                 vk::Offset3D(),
                 vk::Offset3D( static_cast< int32_t >( _source_size.width ), static_cast< int32_t >( _source_size.height ), 1 ),
				 }
			};
			const std::array< vk::Offset3D, 2 > destination{
				{
                 vk::Offset3D(),
                 vk::Offset3D( static_cast< int32_t >( _destination_size.width ), static_cast< int32_t >( _destination_size.height ), 1 ),
				 }
			};

			const vk::ImageBlit2 blit_region( vk::ImageSubresourceLayers( vk::ImageAspectFlagBits::eColor, 0, 0, 1 ),
			                                  source,
			                                  vk::ImageSubresourceLayers( vk::ImageAspectFlagBits::eColor, 0, 0, 1 ),
			                                  destination );

			const vk::BlitImageInfo2
				blit_info( _source, vk::ImageLayout::eTransferSrcOptimal, _destination, vk::ImageLayout::eTransferDstOptimal, 1, &blit_region, vk::Filter::eLinear );
			_command_buffer.blitImage2( blit_info );
		}

		vk::ShaderModule createShaderModule( const std::string& _name )
		{
			ZoneScoped;

			const cRenderer_vulkan* renderer = reinterpret_cast< cRenderer_vulkan* >( cRenderer::getRenderInstance() );

			std::vector< char > shader;
			vk::ShaderModule    module = nullptr;

			std::fstream shader_file = filesystem::open( fmt::format( "binaries/shaders/vulkan/{}.spv", _name ), std::ios::in | std::ios::ate | std::ios::binary );
			if( !shader_file.is_open() )
			{
				DF_LOG_ERROR( fmt::format( "Failed to load shader: {}", _name ) );
				return module;
			}

			shader.resize( shader_file.tellg() );
			shader_file.seekg( 0 );
			shader_file.read( shader.data(), static_cast< long long >( shader.size() ) );

			const vk::ShaderModuleCreateInfo create_info( vk::ShaderModuleCreateFlags(), shader.size(), reinterpret_cast< const uint32_t* >( shader.data() ) );

			module = renderer->getLogicalDevice().createShaderModule( create_info ).value;
			DF_LOG_MESSAGE( fmt::format( "Successfully loaded shader and created shader module: {}", _name ) );
			return module;
		}

		void createBuffer( const vk::DeviceSize _size, const vk::BufferUsageFlags _usage_flags, const vma::MemoryUsage _memory_usage, sAllocatedBuffer_vulkan& _buffer )
		{
			ZoneScoped;

			createBuffer( _size, _usage_flags, _memory_usage, _buffer, reinterpret_cast< cRenderer_vulkan* >( cRenderer::getRenderInstance() )->getMemoryAllocator() );
		}

		void createBuffer( const vk::DeviceSize       _size,
		                   const vk::BufferUsageFlags _usage_flags,
		                   const vma::MemoryUsage     _memory_usage,
		                   sAllocatedBuffer_vulkan&   _buffer,
		                   const vma::Allocator&      _memory_allocator )
		{
			ZoneScoped;

			const vk::BufferCreateInfo buffer_create_info( vk::BufferCreateFlags(), _size, _usage_flags );

			const vma::AllocationCreateInfo allocation_create_info( vma::AllocationCreateFlagBits::eMapped, _memory_usage );

			std::pair< vma::UniqueBuffer, vma::UniqueAllocation > value = _memory_allocator.createBufferUnique( buffer_create_info, allocation_create_info ).value;
			_buffer.buffer.swap( value.first );
			_buffer.allocation.swap( value.second );
		}

		sAllocatedBuffer_vulkan createBuffer( const vk::DeviceSize _size, const vk::BufferUsageFlags _usage_flags, const vma::MemoryUsage _memory_usage )
		{
			ZoneScoped;

			sAllocatedBuffer_vulkan buffer{};
			createBuffer( _size, _usage_flags, _memory_usage, buffer );
			return buffer;
		}

		sAllocatedBuffer_vulkan createBuffer( const vk::DeviceSize       _size,
		                                      const vk::BufferUsageFlags _usage_flags,
		                                      const vma::MemoryUsage     _memory_usage,
		                                      const vma::Allocator&      _memory_allocator )
		{
			ZoneScoped;

			sAllocatedBuffer_vulkan buffer{};
			createBuffer( _size, _usage_flags, _memory_usage, buffer, _memory_allocator );
			return buffer;
		}

		void destroyBuffer( sAllocatedBuffer_vulkan& _buffer )
		{
			ZoneScoped;

			const cRenderer_vulkan* renderer = reinterpret_cast< cRenderer_vulkan* >( cRenderer::getRenderInstance() );

			renderer->getMemoryAllocator().destroyBuffer( _buffer.buffer.get(), _buffer.allocation.get() );
			_buffer.buffer.release();
			_buffer.allocation.release();
		}

		void destroyBuffer( sAllocatedBuffer_vulkan& _buffer, const vma::Allocator& _memory_allocator )
		{
			ZoneScoped;

			_memory_allocator.destroyBuffer( _buffer.buffer.get(), _buffer.allocation.get() );
			_buffer.buffer.release();
			_buffer.allocation.release();
		}

		sAllocatedImage_vulkan createImage( const vk::Extent3D _size, const vk::Format _format, const vk::ImageUsageFlags _usage, const bool _mipmapped, const unsigned _mipmaps )
		{
			ZoneScoped;

			const cRenderer_vulkan* renderer = reinterpret_cast< cRenderer_vulkan* >( cRenderer::getRenderInstance() );

			sAllocatedImage_vulkan image{
				.extent = _size,
				.format = _format,
			};

			vk::ImageCreateInfo image_create_info = init::imageCreateInfo( _format, _usage, _size );
			if( _mipmapped )
			{
				if( _mipmaps >= 1 )
					image_create_info.mipLevels = _mipmaps;
				else
					image_create_info.mipLevels = static_cast< uint32_t >( std::floor( std::log2( std::max( _size.width, _size.height ) ) ) ) + 1;
			}

			constexpr vma::AllocationCreateInfo allocation_create_info( vma::AllocationCreateFlags(), vma::MemoryUsage::eGpuOnly, vk::MemoryPropertyFlagBits::eDeviceLocal );

			std::pair< vma::UniqueImage, vma::UniqueAllocation > value = renderer->getMemoryAllocator().createImageUnique( image_create_info, allocation_create_info ).value;
			image.image.swap( value.first );
			image.allocation.swap( value.second );

			vk::ImageAspectFlags aspect_flags = vk::ImageAspectFlagBits::eColor;
			if( _format == vk::Format::eD32Sfloat )
				aspect_flags = vk::ImageAspectFlagBits::eDepth;

			vk::ImageViewCreateInfo image_view_create_info     = init::imageViewCreateInfo( _format, image.image.get(), aspect_flags );
			image_view_create_info.subresourceRange.levelCount = image_create_info.mipLevels;

			image.image_view = renderer->getLogicalDevice().createImageViewUnique( image_view_create_info ).value;
			return image;
		}

		sAllocatedImage_vulkan createImage( const void*               _data,
		                                    const vk::Extent3D        _size,
		                                    const vk::Format          _format,
		                                    const vk::ImageUsageFlags _usage,
		                                    const bool                _mipmapped,
		                                    const unsigned            _mipmaps )
		{
			ZoneScoped;

			const cRenderer_vulkan* renderer = reinterpret_cast< cRenderer_vulkan* >( cRenderer::getRenderInstance() );

			const uint32_t          data_size = _size.depth * _size.width * _size.height * 4;
			sAllocatedBuffer_vulkan buffer    = createBuffer( data_size, vk::BufferUsageFlagBits::eTransferSrc, vma::MemoryUsage::eCpuToGpu );

			void* data_dst = renderer->getMemoryAllocator().mapMemory( buffer.allocation.get() ).value;
			std::memcpy( data_dst, _data, data_size );
			renderer->getMemoryAllocator().unmapMemory( buffer.allocation.get() );

			sAllocatedImage_vulkan image = createImage( _size,
			                                            _format,
			                                            _usage | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eTransferSrc,
			                                            _mipmapped,
			                                            _mipmaps );

			renderer->immediateSubmit(
				[ & ]( const vk::CommandBuffer _command_buffer )
				{
					transitionImage( _command_buffer, image.image.get(), vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal );

					const vk::BufferImageCopy region( 0, 0, 0, vk::ImageSubresourceLayers( vk::ImageAspectFlagBits::eColor, 0, 0, 1 ), vk::Offset3D(), _size );

					_command_buffer.copyBufferToImage( buffer.buffer.get(), image.image.get(), vk::ImageLayout::eTransferDstOptimal, 1, &region );

					transitionImage( _command_buffer, image.image.get(), vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal );
				} );

			return image;
		}

		void destroyImage( sAllocatedImage_vulkan& _image )
		{
			ZoneScoped;

			const cRenderer_vulkan* renderer = reinterpret_cast< cRenderer_vulkan* >( cRenderer::getRenderInstance() );

			_image.image_view.reset();
			renderer->getMemoryAllocator().destroyImage( _image.image.get(), _image.allocation.get() );
			_image.image.release();
			_image.allocation.release();
		}
	}
}

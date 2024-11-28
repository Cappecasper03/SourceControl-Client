#include "cRenderer_vulkan.h"

#include "engine/rendering/cRenderer.h"

#define GLFW_INCLUDE_VULKAN
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <vulkan/vulkan_to_string.hpp>

#include "descriptor/sDescriptorLayoutBuilder_vulkan.h"
#include "engine/managers/assets/cCameraManager.h"
#include "engine/managers/cEventManager.h"
#include "misc/Helper_vulkan.h"

namespace df::vulkan
{
	cRenderer_vulkan::cRenderer_vulkan( const std::string& _window_name )
		: m_frames_in_flight( 3 )
		, m_frame_number( 0 )
		, m_frame_datas( m_frames_in_flight )
	{
		ZoneScoped;

		glfwInit();

		glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );

		m_window = glfwCreateWindow( m_window_size.x, m_window_size.y, _window_name.data(), nullptr, nullptr );
		if( !m_window )
			DF_LOG_ERROR( "Failed to create window" );
		else
			DF_LOG_MESSAGE( fmt::format( "Created window [{}, {}]", m_window_size.x, m_window_size.y ) );

		glfwSetWindowUserPointer( m_window, this );
		glfwSetFramebufferSizeCallback( m_window, framebufferSizeCallback );

		VULKAN_HPP_DEFAULT_DISPATCHER.init();

		uint32_t     extension_count;
		const char** required_extensions = glfwGetRequiredInstanceExtensions( &extension_count );

		const std::vector instance_layer_names     = { "VK_LAYER_KHRONOS_validation" };
		std::vector       instance_extension_names = { vk::EXTDebugUtilsExtensionName };

		for( uint32_t i = 0; i < extension_count; ++i )
			instance_extension_names.push_back( required_extensions[ i ] );

		const vk::ApplicationInfo    application_info( _window_name.data(), 0, "DragonForge", 0, vk::ApiVersion13 );
		const vk::InstanceCreateInfo instance_create_info( vk::InstanceCreateFlags(), &application_info, instance_layer_names, instance_extension_names );
		m_instance = createInstanceUnique( instance_create_info ).value;

		VULKAN_HPP_DEFAULT_DISPATCHER.init( m_instance.get() );

		const vk::DebugUtilsMessageSeverityFlagsEXT severity_flags( vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
		                                                            | vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose );
		const vk::DebugUtilsMessageTypeFlagsEXT     message_type_flags( vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
                                                                    | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation );
		const vk::DebugUtilsMessengerCreateInfoEXT  debug_create_info( vk::DebugUtilsMessengerCreateFlagsEXT(),
                                                                      severity_flags,
                                                                      message_type_flags,
                                                                      &cRenderer_vulkan::debugMessageCallback );
		m_debug_messenger = m_instance->createDebugUtilsMessengerEXTUnique( debug_create_info ).value;

		VkSurfaceKHR temp_surface{};
		glfwCreateWindowSurface( m_instance.get(), m_window, nullptr, &temp_surface );
		m_surface = vk::UniqueSurfaceKHR( temp_surface, m_instance.get() );

		m_physical_device = m_instance->enumeratePhysicalDevices().value.front();

		const std::vector< vk::QueueFamilyProperties > queue_family_properties = m_physical_device.getQueueFamilyProperties();

		const auto it = std::find_if( queue_family_properties.begin(),
		                              queue_family_properties.end(),
		                              []( vk::QueueFamilyProperties const& _properties ) { return _properties.queueFlags & vk::QueueFlagBits::eGraphics; } );

		m_graphics_queue_family = static_cast< uint32_t >( std::distance( queue_family_properties.begin(), it ) );

		std::vector device_extension_names = { vk::KHRSwapchainExtensionName };
#ifdef PROFILING
		device_extension_names.push_back( vk::KHRCalibratedTimestampsExtensionName );
#endif

		vk::PhysicalDeviceSynchronization2Features    synchronization2_features( true );
		vk::PhysicalDeviceBufferDeviceAddressFeatures buffer_device_address_features( true, false, false, &synchronization2_features );
		vk::PhysicalDeviceDynamicRenderingFeatures    dynamic_rendering_features( true, &buffer_device_address_features );

		constexpr float           queue_priority = 0;
		vk::DeviceQueueCreateInfo device_queue_create_info( vk::DeviceQueueCreateFlags(), m_graphics_queue_family, 1, &queue_priority );
		m_logical_device = m_physical_device
		                       .createDeviceUnique(
								   vk::DeviceCreateInfo( vk::DeviceCreateFlags(), device_queue_create_info, {}, device_extension_names, {}, &dynamic_rendering_features ) )
		                       .value;
		m_graphics_queue = m_logical_device->getQueue( m_graphics_queue_family, 0 );

		VULKAN_HPP_DEFAULT_DISPATCHER.init( m_logical_device.get() );

		createMemoryAllocator();
		createSwapchain( m_window_size.x, m_window_size.y );
		createFrameDatas();
		createSubmitContext();

		sDescriptorLayoutBuilder_vulkan layout_builder;
		layout_builder.addBinding( 0, vk::DescriptorType::eUniformBuffer );
		m_vertex_scene_uniform_layout = layout_builder.build( m_logical_device.get(), vk::ShaderStageFlagBits::eVertex );

		m_sampler_linear  = m_logical_device->createSamplerUnique( vk::SamplerCreateInfo( vk::SamplerCreateFlags(), vk::Filter::eLinear, vk::Filter::eLinear ) ).value;
		m_sampler_nearest = m_logical_device->createSamplerUnique( vk::SamplerCreateInfo( vk::SamplerCreateFlags(), vk::Filter::eNearest, vk::Filter::eNearest ) ).value;

		DF_LOG_MESSAGE( "Initialized renderer" );
	}

	cRenderer_vulkan::~cRenderer_vulkan()
	{
		ZoneScoped;

		if( m_logical_device->waitIdle() != vk::Result::eSuccess )
			DF_LOG_ERROR( "Failed to wait for device idle" );

		if( ImGui::GetCurrentContext() )
		{
			ImGui_ImplVulkan_Shutdown();
			m_imgui_descriptor_pool.reset();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext();
		}

		m_sampler_nearest.reset();
		m_sampler_linear.reset();

		m_vertex_scene_uniform_layout.reset();

		TracyVkDestroy( m_submit_context.tracy_context );
		m_submit_context.command_buffer.reset();
		m_submit_context.command_pool.reset();
		m_submit_context.fence.reset();

		for( sFrameData_vulkan& frame_data: m_frame_datas )
		{
			TracyVkDestroy( frame_data.tracy_context );

			frame_data.descriptors.destroy();

			helper::util::destroyBuffer( frame_data.vertex_scene_uniform_buffer_2d );
			helper::util::destroyBuffer( frame_data.vertex_scene_uniform_buffer_3d );

			frame_data.render_fence.reset();
			frame_data.render_semaphore.reset();
			frame_data.swapchain_semaphore.reset();

			frame_data.command_buffer.reset();
			frame_data.command_pool.reset();
		}

		helper::util::destroyImage( m_render_image );
		helper::util::destroyImage( m_depth_image );

		for( vk::UniqueImageView& swapchain_image_view: m_swapchain_image_views )
			swapchain_image_view.reset();

		m_swapchain.reset();

		memory_allocator.reset();

		m_logical_device.reset();

		m_surface.reset();

		m_debug_messenger.reset();

		m_instance.reset();

		glfwDestroyWindow( m_window );

		glfwTerminate();

		DF_LOG_MESSAGE( "Deinitialized renderer" );
	}

	void cRenderer_vulkan::render()
	{
		ZoneScoped;

		sFrameData_vulkan&             frame_data     = getCurrentFrame();
		const vk::UniqueCommandBuffer& command_buffer = frame_data.command_buffer;

		vk::Result result = m_logical_device->waitForFences( 1, &frame_data.render_fence.get(), true, std::numeric_limits< uint64_t >::max() );
		frame_data.descriptors.clear();
		if( result != vk::Result::eSuccess )
			DF_LOG_ERROR( "Failed to wait for fences" );

		uint32_t swapchain_image_index;
		result = m_logical_device->acquireNextImageKHR( m_swapchain.get(),
		                                                std::numeric_limits< uint64_t >::max(),
		                                                frame_data.swapchain_semaphore.get(),
		                                                nullptr,
		                                                &swapchain_image_index );

		if( result == vk::Result::eErrorOutOfDateKHR )
		{
			resize();
			return;
		}

		if( result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR )
		{
			DF_LOG_ERROR( "Failed to acquire next image" );
			return;
		}

		result = m_logical_device->resetFences( 1, &frame_data.render_fence.get() );
		if( result != vk::Result::eSuccess )
			DF_LOG_ERROR( "Failed to reset fences" );

		m_logical_device->resetCommandPool( frame_data.command_pool.get() );

		const vk::CommandBufferBeginInfo command_buffer_begin_info = helper::init::commandBufferBeginInfo();

		if( command_buffer->begin( &command_buffer_begin_info ) != vk::Result::eSuccess )
		{
			DF_LOG_ERROR( "Failed to begin command buffer" );
			return;
		}

		{
			TracyVkZone( frame_data.tracy_context, command_buffer.get(), __FUNCTION__ );

			helper::util::transitionImage( command_buffer.get(), m_render_image.image.get(), vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral );

			if( cRenderer::isDeferred() )
				renderDeferred( command_buffer.get() );
			else
			{
				cEventManager::invoke( event::render_3d );
				cEventManager::invoke( event::render_2d );
			}

			helper::util::transitionImage( command_buffer.get(), m_render_image.image.get(), vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferSrcOptimal );
			helper::util::transitionImage( command_buffer.get(), m_swapchain_images[ swapchain_image_index ], vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal );

			helper::util::copyImageToImage( command_buffer.get(), m_render_image.image.get(), m_swapchain_images[ swapchain_image_index ], m_render_extent, m_swapchain_extent );

			if( ImGui::GetCurrentContext() )
			{
				ImGui_ImplVulkan_NewFrame();
				ImGui_ImplGlfw_NewFrame();
				ImGui::NewFrame();
				cEventManager::invoke( event::imgui );
				ImGui::Render();

				const vk::RenderingAttachmentInfo color_attachment = helper::init::attachmentInfo( m_swapchain_image_views[ swapchain_image_index ].get(),
				                                                                                   nullptr,
				                                                                                   vk::ImageLayout::eColorAttachmentOptimal );
				const vk::RenderingInfo           render_info      = helper::init::renderingInfo( m_swapchain_extent, &color_attachment );

				command_buffer->beginRendering( &render_info );

				ImGui_ImplVulkan_RenderDrawData( ImGui::GetDrawData(), command_buffer.get() );
				command_buffer->endRendering();
			}

			helper::util::transitionImage( command_buffer.get(),
			                               m_swapchain_images[ swapchain_image_index ],
			                               vk::ImageLayout::eTransferDstOptimal,
			                               vk::ImageLayout::ePresentSrcKHR );
		}

		TracyVkCollect( frame_data.tracy_context, command_buffer.get() );

		if( command_buffer->end() != vk::Result::eSuccess )
			DF_LOG_ERROR( "Failed to end command buffer" );

		const vk::CommandBufferSubmitInfo command_buffer_submit_info   = helper::init::commandBufferSubmitInfo( command_buffer.get() );
		const vk::SemaphoreSubmitInfo     wait_semaphore_submit_info   = helper::init::semaphoreSubmitInfo( vk::PipelineStageFlagBits2::eColorAttachmentOutput,
                                                                                                      frame_data.swapchain_semaphore.get() );
		const vk::SemaphoreSubmitInfo     signal_semaphore_submit_info = helper::init::semaphoreSubmitInfo( vk::PipelineStageFlagBits2::eAllGraphics,
                                                                                                        frame_data.render_semaphore.get() );
		const vk::SubmitInfo2             submit_info = helper::init::submitInfo( &command_buffer_submit_info, &signal_semaphore_submit_info, &wait_semaphore_submit_info );

		if( m_graphics_queue.submit2( 1, &submit_info, frame_data.render_fence.get() ) != vk::Result::eSuccess )
		{
			DF_LOG_WARNING( "Failed to submit render queue" );
			return;
		}

		const vk::PresentInfoKHR present_info = helper::init::presentInfo( &frame_data.render_semaphore.get(), &m_swapchain.get(), &swapchain_image_index );

		result = m_graphics_queue.presentKHR( &present_info );
		if( result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR || m_window_resized )
			resize();
		else if( result != vk::Result::eSuccess )
		{
			DF_LOG_ERROR( "Queue present failed" );
			return;
		}

		m_frame_number++;
	}

	void cRenderer_vulkan::beginRendering( const int _clear_buffers, const cColor& _color )
	{
		ZoneScoped;
		const sFrameData_vulkan& frame_data = getCurrentFrame();
		TracyVkZone( frame_data.tracy_context, frame_data.command_buffer.get(), __FUNCTION__ );

		const bool color = _clear_buffers & cCamera::eClearBuffer::eColor;
		const bool depth = _clear_buffers & cCamera::eClearBuffer::eDepth;

		const vk::UniqueCommandBuffer& command_buffer = frame_data.command_buffer;
		const vk::ClearValue           clear_color_value( vk::ClearColorValue( _color.r, _color.g, _color.b, _color.a ) );
		constexpr vk::ClearValue       clear_depth_stencil_value( vk::ClearDepthStencilValue( 1 ) );

		const vk::RenderingAttachmentInfo color_attachment = helper::init::attachmentInfo( m_render_image.image_view.get(),
		                                                                                   color ? &clear_color_value : nullptr,
		                                                                                   vk::ImageLayout::eColorAttachmentOptimal );

		const vk::RenderingAttachmentInfo depth_attachment = helper::init::attachmentInfo( m_depth_image.image_view.get(),
		                                                                                   depth ? &clear_depth_stencil_value : nullptr,
		                                                                                   vk::ImageLayout::eDepthAttachmentOptimal );

		const vk::RenderingInfo rendering_info = helper::init::renderingInfo( m_render_extent, &color_attachment, &depth_attachment );
		command_buffer->beginRendering( &rendering_info );
	}

	void cRenderer_vulkan::endRendering()
	{
		ZoneScoped;
		const sFrameData_vulkan& frame_data = getCurrentFrame();
		TracyVkZone( frame_data.tracy_context, frame_data.command_buffer.get(), __FUNCTION__ );

		const vk::UniqueCommandBuffer& command_buffer = frame_data.command_buffer;
		command_buffer->endRendering();
	}

	void cRenderer_vulkan::immediateSubmit( const std::function< void( vk::CommandBuffer ) >& _function ) const
	{
		ZoneScoped;

		if( m_logical_device->resetFences( 1, &m_submit_context.fence.get() ) != vk::Result::eSuccess )
			DF_LOG_ERROR( "Failed to reset fences" );

		if( m_logical_device->resetCommandPool( m_submit_context.command_pool.get() ) != vk::Result::eSuccess )
			DF_LOG_ERROR( "Failed to reset command pool" );

		const vk::UniqueCommandBuffer&   command_buffer            = m_submit_context.command_buffer;
		const vk::CommandBufferBeginInfo command_buffer_begin_info = helper::init::commandBufferBeginInfo( vk::CommandBufferUsageFlagBits::eOneTimeSubmit );

		if( command_buffer->begin( &command_buffer_begin_info ) != vk::Result::eSuccess )
		{
			DF_LOG_ERROR( "Failed to begin command buffer" );
			return;
		}

		{
			TracyVkZone( m_submit_context.tracy_context, m_submit_context.command_buffer.get(), __FUNCTION__ );

			_function( command_buffer.get() );
		}

		TracyVkCollect( m_submit_context.tracy_context, m_submit_context.command_buffer.get() );

		if( command_buffer->end() != vk::Result::eSuccess )
			DF_LOG_ERROR( "Failed to end command buffer" );

		const vk::CommandBufferSubmitInfo buffer_submit_info = helper::init::commandBufferSubmitInfo( command_buffer.get() );
		const vk::SubmitInfo2             submit_info        = helper::init::submitInfo( &buffer_submit_info );

		if( m_graphics_queue.submit2( 1, &submit_info, m_submit_context.fence.get() ) != vk::Result::eSuccess )
		{
			DF_LOG_WARNING( "Failed to submit render queue" );
			return;
		}

		if( m_logical_device->waitForFences( 1, &m_submit_context.fence.get(), true, std::numeric_limits< uint64_t >::max() ) != vk::Result::eSuccess )
			DF_LOG_ERROR( "Failed to wait for fences" );
	}

	void cRenderer_vulkan::setViewport()
	{
		const vk::Viewport viewport( 0, 0, static_cast< float >( m_render_extent.width ), static_cast< float >( m_render_extent.height ), 0, 1 );
		getCurrentFrame().command_buffer->setViewport( 0, 1, &viewport );
	}

	void cRenderer_vulkan::setScissor()
	{
		const vk::Rect2D scissor( vk::Offset2D(), vk::Extent2D( m_render_extent.width, m_render_extent.height ) );
		getCurrentFrame().command_buffer->setScissor( 0, 1, &scissor );
	}

	void cRenderer_vulkan::setViewportScissor()
	{
		ZoneScoped;

		setViewport();
		setScissor();
	}

	void cRenderer_vulkan::initializeImGui()
	{
		ZoneScoped;

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io     = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

		ImGui_ImplGlfw_InitForOpenGL( m_window, true );

		const std::vector pool_sizes = {
			vk::DescriptorPoolSize( vk::DescriptorType::eSampler, 1000 ),
			vk::DescriptorPoolSize( vk::DescriptorType::eCombinedImageSampler, 1000 ),
			vk::DescriptorPoolSize( vk::DescriptorType::eSampledImage, 1000 ),
			vk::DescriptorPoolSize( vk::DescriptorType::eStorageImage, 1000 ),
			vk::DescriptorPoolSize( vk::DescriptorType::eUniformTexelBuffer, 1000 ),
			vk::DescriptorPoolSize( vk::DescriptorType::eStorageTexelBuffer, 1000 ),
			vk::DescriptorPoolSize( vk::DescriptorType::eUniformBuffer, 1000 ),
			vk::DescriptorPoolSize( vk::DescriptorType::eStorageBuffer, 1000 ),
			vk::DescriptorPoolSize( vk::DescriptorType::eUniformBufferDynamic, 1000 ),
			vk::DescriptorPoolSize( vk::DescriptorType::eStorageBufferDynamic, 1000 ),
			vk::DescriptorPoolSize( vk::DescriptorType::eInputAttachment, 1000 ),
		};

		const vk::DescriptorPoolCreateInfo create_info( vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet, static_cast< uint32_t >( 1000 ), pool_sizes );

		m_imgui_descriptor_pool = m_logical_device->createDescriptorPoolUnique( create_info ).value;

		ImGui_ImplVulkan_InitInfo init_info{
			.Instance                    = m_instance.get(),
			.PhysicalDevice              = m_physical_device,
			.Device                      = m_logical_device.get(),
			.QueueFamily                 = m_graphics_queue_family,
			.Queue                       = m_graphics_queue,
			.DescriptorPool              = m_imgui_descriptor_pool.get(),
			.MinImageCount               = 3,
			.ImageCount                  = 3,
			.MSAASamples                 = static_cast< VkSampleCountFlagBits >( vk::SampleCountFlagBits::e1 ),
			.UseDynamicRendering         = true,
			.PipelineRenderingCreateInfo = {
				.sType					 = static_cast< VkStructureType >( vk::StructureType::ePipelineRenderingCreateInfo ),
				.colorAttachmentCount	 = 1,
				.pColorAttachmentFormats = reinterpret_cast< const VkFormat* >( &m_swapchain_format ),
			},
		};
		ImGui_ImplVulkan_Init( &init_info );
	}

	void cRenderer_vulkan::createSwapchain( const uint32_t _width, const uint32_t _height )
	{
		ZoneScoped;

		const std::vector< vk::SurfaceFormatKHR > formats              = m_physical_device.getSurfaceFormatsKHR( m_surface.get() ).value;
		const vk::SurfaceCapabilitiesKHR          surface_capabilities = m_physical_device.getSurfaceCapabilitiesKHR( m_surface.get() ).value;

		m_swapchain_format = formats.front().format == vk::Format::eUndefined ? vk::Format::eB8G8R8A8Unorm : formats.front().format;

		if( surface_capabilities.currentExtent.width == std::numeric_limits< uint32_t >::max() )
		{
			m_swapchain_extent.width  = std::clamp( _width, surface_capabilities.minImageExtent.width, surface_capabilities.maxImageExtent.width );
			m_swapchain_extent.height = std::clamp( _height, surface_capabilities.minImageExtent.height, surface_capabilities.maxImageExtent.height );
		}
		else
			m_swapchain_extent = surface_capabilities.currentExtent;

		const std::vector family_indices = { m_graphics_queue_family };

		vk::SwapchainCreateInfoKHR swapchain_create_info( vk::SwapchainCreateFlagsKHR(),
		                                                  m_surface.get(),
		                                                  std::clamp( m_frames_in_flight, surface_capabilities.minImageCount, surface_capabilities.maxImageCount ),
		                                                  m_swapchain_format,
		                                                  vk::ColorSpaceKHR::eSrgbNonlinear,
		                                                  m_swapchain_extent,
		                                                  1,
		                                                  vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst,
		                                                  vk::SharingMode::eExclusive,
		                                                  family_indices,
		                                                  vk::SurfaceTransformFlagBitsKHR::eIdentity,
		                                                  vk::CompositeAlphaFlagBitsKHR::eOpaque,
		                                                  vk::PresentModeKHR::eMailbox,
		                                                  true );

		m_swapchain = m_logical_device->createSwapchainKHRUnique( swapchain_create_info ).value;

		for( vk::Image image: m_logical_device->getSwapchainImagesKHR( m_swapchain.get() ).value )
		{
			m_swapchain_images.emplace_back( image );

			vk::ImageViewCreateInfo image_view_create_info( vk::ImageViewCreateFlags(),
			                                                image,
			                                                vk::ImageViewType::e2D,
			                                                m_swapchain_format,
			                                                vk::ComponentMapping(),
			                                                vk::ImageSubresourceRange( vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 ) );
			m_swapchain_image_views.push_back( m_logical_device->createImageViewUnique( image_view_create_info ).value );
		}

		m_render_extent = vk::Extent2D( m_swapchain_extent.width, m_swapchain_extent.height );
		m_depth_image   = {
			  .extent = vk::Extent3D( m_render_extent.width, m_render_extent.height, 1 ),
			  .format = vk::Format::eD32Sfloat,
		};
		m_render_image = {
			.extent = vk::Extent3D( m_render_extent.width, m_render_extent.height, 1 ),
			.format = vk::Format::eR16G16B16A16Sfloat,
		};

		constexpr vk::ImageUsageFlags depth_usage_flags       = vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eTransferDst;
		const vk::ImageCreateInfo     depth_image_create_info = helper::init::imageCreateInfo( m_depth_image.format, depth_usage_flags, m_depth_image.extent );

		constexpr vk::ImageUsageFlags render_usage_flags = vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eStorage
		                                                 | vk::ImageUsageFlagBits::eColorAttachment;
		const vk::ImageCreateInfo render_image_create_info = helper::init::imageCreateInfo( m_render_image.format, render_usage_flags, m_render_image.extent );

		constexpr vma::AllocationCreateInfo allocation_create_info( vma::AllocationCreateFlags(), vma::MemoryUsage::eGpuOnly, vk::MemoryPropertyFlagBits::eDeviceLocal );

		std::pair< vma::UniqueImage, vma::UniqueAllocation > depth = memory_allocator->createImageUnique( depth_image_create_info, allocation_create_info ).value;
		m_depth_image.image.swap( depth.first );
		m_depth_image.allocation.swap( depth.second );

		std::pair< vma::UniqueImage, vma::UniqueAllocation > render = memory_allocator->createImageUnique( render_image_create_info, allocation_create_info ).value;
		m_render_image.image.swap( render.first );
		m_render_image.allocation.swap( render.second );

		vk::ImageViewCreateInfo depth_image_view_create_info  = helper::init::imageViewCreateInfo( m_depth_image.format,
                                                                                                  m_depth_image.image.get(),
                                                                                                  vk::ImageAspectFlagBits::eDepth );
		vk::ImageViewCreateInfo render_image_view_create_info = helper::init::imageViewCreateInfo( m_render_image.format,
		                                                                                           m_render_image.image.get(),
		                                                                                           vk::ImageAspectFlagBits::eColor );

		m_depth_image.image_view  = m_logical_device->createImageViewUnique( depth_image_view_create_info ).value;
		m_render_image.image_view = m_logical_device->createImageViewUnique( render_image_view_create_info ).value;
	}

	void cRenderer_vulkan::createFrameDatas()
	{
		ZoneScoped;

		const vk::CommandPoolCreateInfo command_pool_create_info = helper::init::commandPoolCreateInfo( m_graphics_queue_family );
		const vk::SemaphoreCreateInfo   semaphore_create_info    = helper::init::semaphoreCreateInfo();
		const vk::FenceCreateInfo       fence_create_info        = helper::init::fenceCreateInfo();

		for( sFrameData_vulkan& frame_data: m_frame_datas )
		{
			frame_data.command_pool = m_logical_device->createCommandPoolUnique( command_pool_create_info ).value;

			vk::CommandBufferAllocateInfo command_buffer_allocate_info = helper::init::commandBufferAllocateInfo( frame_data.command_pool.get() );
			frame_data.command_buffer.swap( m_logical_device->allocateCommandBuffersUnique( command_buffer_allocate_info ).value.front() );

			frame_data.swapchain_semaphore = m_logical_device->createSemaphoreUnique( semaphore_create_info ).value;
			frame_data.render_semaphore    = m_logical_device->createSemaphoreUnique( semaphore_create_info ).value;
			frame_data.render_fence        = m_logical_device->createFenceUnique( fence_create_info ).value;

			frame_data.vertex_scene_uniform_buffer_3d = helper::util::createBuffer( sizeof( sVertexSceneUniforms_vulkan ),
			                                                                        vk::BufferUsageFlagBits::eUniformBuffer,
			                                                                        vma::MemoryUsage::eCpuToGpu,
			                                                                        memory_allocator.get() );
			frame_data.vertex_scene_uniform_buffer_2d = helper::util::createBuffer( sizeof( sVertexSceneUniforms_vulkan ),
			                                                                        vk::BufferUsageFlagBits::eUniformBuffer,
			                                                                        vma::MemoryUsage::eCpuToGpu,
			                                                                        memory_allocator.get() );

			std::vector< sDescriptorAllocator_vulkan::sPoolSizeRatio > frame_sizes{
				{vk::DescriptorType::eStorageImage,          3},
				{ vk::DescriptorType::eStorageBuffer,        3},
				{ vk::DescriptorType::eUniformBuffer,        3},
				{ vk::DescriptorType::eCombinedImageSampler, 3},
			};

			frame_data.descriptors.create( m_logical_device.get(), 1000, frame_sizes );

#ifdef PROFILING
			m_get_instance_proc_addr = reinterpret_cast< PFN_vkGetInstanceProcAddr >( m_instance->getProcAddr( "vkGetInstanceProcAddr" ) );
			m_get_device_proc_addr   = reinterpret_cast< PFN_vkGetDeviceProcAddr >( m_instance->getProcAddr( "vkGetDeviceProcAddr" ) );

			frame_data.tracy_context = TracyVkContextCalibrated( m_instance.get(),
			                                                     m_physical_device,
			                                                     m_logical_device.get(),
			                                                     m_graphics_queue,
			                                                     frame_data.command_buffer.get(),
			                                                     m_get_instance_proc_addr,
			                                                     m_get_device_proc_addr );
#endif
		}
	}

	void cRenderer_vulkan::createMemoryAllocator()
	{
		ZoneScoped;

		vma::AllocatorCreateInfo create_info( vma::AllocatorCreateFlagBits::eExtMemoryBudget, m_physical_device, m_logical_device.get() );
		create_info.setInstance( m_instance.get() );
		create_info.setVulkanApiVersion( vk::ApiVersion13 );

		memory_allocator = createAllocatorUnique( create_info ).value;
		DF_LOG_MESSAGE( "Created memory allocator" );
	}

	void cRenderer_vulkan::createSubmitContext()
	{
		ZoneScoped;

		m_submit_context.fence = m_logical_device->createFenceUnique( helper::init::fenceCreateInfo() ).value;

		const vk::CommandPoolCreateInfo create_info = helper::init::commandPoolCreateInfo( m_graphics_queue_family );
		m_submit_context.command_pool               = m_logical_device->createCommandPoolUnique( create_info ).value;

		const vk::CommandBufferAllocateInfo allocate_info = helper::init::commandBufferAllocateInfo( m_submit_context.command_pool.get() );
		m_submit_context.command_buffer.swap( m_logical_device->allocateCommandBuffersUnique( allocate_info ).value.front() );

#ifdef PROFILING
		m_submit_context.tracy_context = TracyVkContextCalibrated( m_instance.get(),
		                                                           m_physical_device,
		                                                           m_logical_device.get(),
		                                                           m_graphics_queue,
		                                                           m_submit_context.command_buffer.get(),
		                                                           m_get_instance_proc_addr,
		                                                           m_get_device_proc_addr );
#endif

		DF_LOG_MESSAGE( "Created submit context" );
	}

	void cRenderer_vulkan::resize()
	{
		ZoneScoped;

		int width = 0, height = 0;
		while( width == 0 || height == 0 )
		{
			glfwGetFramebufferSize( m_window, &width, &height );
			glfwWaitEvents();
		}

		if( m_logical_device->waitIdle() != vk::Result::eSuccess )
			DF_LOG_ERROR( "Failed to wait for device idle" );

		m_window_size.x = width;
		m_window_size.y = height;

		helper::util::destroyImage( m_render_image );
		helper::util::destroyImage( m_depth_image );

		for( vk::UniqueImageView& swapchain_image_view: m_swapchain_image_views )
			swapchain_image_view.reset();

		m_swapchain.reset();

		m_swapchain_images.clear();
		m_swapchain_image_views.clear();

		createSwapchain( width, height );

		cEventManager::invoke( event::on_window_resize, width, height );
		m_window_resized = false;
		DF_LOG_MESSAGE( fmt::format( "Resized window [{}, {}]", m_window_size.x, m_window_size.y ) );
	}

	void cRenderer_vulkan::framebufferSizeCallback( GLFWwindow* _window, int /*_width*/, int /*_height*/ )
	{
		ZoneScoped;

		cRenderer_vulkan* renderer = static_cast< cRenderer_vulkan* >( glfwGetWindowUserPointer( _window ) );
		renderer->m_window_resized = true;
	}

	VkBool32 cRenderer_vulkan::debugMessageCallback( const VkDebugUtilsMessageSeverityFlagBitsEXT _message_severity,
	                                                 const VkDebugUtilsMessageTypeFlagsEXT        _message_type,
	                                                 const VkDebugUtilsMessengerCallbackDataEXT*  _callback_data,
	                                                 void* /*_user_data*/
	)
	{
		ZoneScoped;

		std::string type = "None";
		if( _message_type >= static_cast< VkDebugUtilsMessageTypeFlagsEXT >( vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance ) )
			type = "Performance";
		else if( _message_type >= static_cast< VkDebugUtilsMessageTypeFlagsEXT >( vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation ) )
			type = "Validation";
		else if( _message_type >= static_cast< VkDebugUtilsMessageTypeFlagsEXT >( vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral ) )
			type = "General";

		if( _message_severity >= static_cast< VkDebugUtilsMessageTypeFlagsEXT >( vk::DebugUtilsMessageSeverityFlagBitsEXT::eError ) )
		{
			DF_LOG_ERROR( fmt::format( "Vulkan, "
			                           "Type: {}, "
			                           "Severity: Error, "
			                           "Message: {}",
			                           type,
			                           _callback_data->pMessage ) );
		}
		else if( _message_severity >= static_cast< VkDebugUtilsMessageTypeFlagsEXT >( vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning ) )
		{
			DF_LOG_WARNING( fmt::format( "Vulkan, "
			                             "Type: {}, "
			                             "Severity: Warning, "
			                             "Message: {}",
			                             type,
			                             _callback_data->pMessage ) );
		}
		else if( _message_severity >= static_cast< VkDebugUtilsMessageTypeFlagsEXT >( vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo ) )
		{
			DF_LOG_MESSAGE( fmt::format( "Vulkan, "
			                             "Type: {}, "
			                             "Severity: Info, "
			                             "Message: {}",
			                             type,
			                             _callback_data->pMessage ) );
		}
		else if( _message_severity >= static_cast< VkDebugUtilsMessageTypeFlagsEXT >( vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose ) )
		{
			DF_LOG_MESSAGE( fmt::format( "Vulkan, "
			                             "Type: {}, "
			                             "Severity: Verbose, "
			                             "Message: {}",
			                             type,
			                             _callback_data->pMessage ) );
		}
		else
		{
			DF_LOG_MESSAGE( fmt::format( "Vulkan, "
			                             "Type: {}, "
			                             "Severity: None, "
			                             "Message: {}",
			                             type,
			                             _callback_data->pMessage ) );
		}

		return false;
	}
}

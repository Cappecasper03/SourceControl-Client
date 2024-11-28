#pragma once

#include <functional>
#include <vector>
#include <vk_mem_alloc.hpp>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_core.h>

#include "engine/misc/Misc.h"
#include "engine/rendering/iRenderer.h"
#include "misc/Types_vulkan.h"

namespace df::vulkan
{
	class cDeferredRenderer_vulkan;

	class cRenderer_vulkan : public iRenderer
	{
	public:
		DF_DISABLE_COPY_AND_MOVE( cRenderer_vulkan )

		explicit cRenderer_vulkan( const std::string& _window_name );
		~cRenderer_vulkan() override;

		void render() override;

		void beginRendering( int _clear_buffers, const cColor& _color ) override;
		void endRendering() override;

		void immediateSubmit( const std::function< void( vk::CommandBuffer ) >& _function ) const;

		void setViewport();
		void setScissor();
		void setViewportScissor();

		void initializeImGui() override;

		vk::Extent2D getRenderExtent() const { return m_render_extent; }
		vk::Format   getRenderColorFormat() const { return m_render_image.format; }
		vk::Format   getRenderDepthFormat() const { return m_depth_image.format; }

		uint32_t           getCurrentFrameIndex() const { return m_frame_number % m_frames_in_flight; }
		sFrameData_vulkan& getCurrentFrame() { return m_frame_datas[ getCurrentFrameIndex() ]; }

		const vk::PhysicalDevice& getPhysicalDevice() const { return m_physical_device; }
		const vk::Device&         getLogicalDevice() const { return m_logical_device.get(); }
		const vma::Allocator&     getMemoryAllocator() const { return memory_allocator.get(); }

		const vk::DescriptorSetLayout& getVertexSceneUniformLayout() const { return m_vertex_scene_uniform_layout.get(); }

		const vk::Sampler& getLinearSampler() const { return m_sampler_linear.get(); }
		const vk::Sampler& getNearestSampler() const { return m_sampler_nearest.get(); }

	protected:
		virtual void renderDeferred( const vk::CommandBuffer& /*_command_buffer*/ ) {}

		void createSwapchain( uint32_t _width, uint32_t _height );
		void createFrameDatas();
		void createMemoryAllocator();
		void createSubmitContext();

		void resize();

		static void framebufferSizeCallback( GLFWwindow* _window, int _width, int _height );

		static VKAPI_ATTR VkBool32 VKAPI_CALL debugMessageCallback( VkDebugUtilsMessageSeverityFlagBitsEXT      _message_severity,
		                                                            VkDebugUtilsMessageTypeFlagsEXT             _message_type,
		                                                            const VkDebugUtilsMessengerCallbackDataEXT* _callback_data,
		                                                            void*                                       _user_data );

		vk::UniqueInstance m_instance;

		vk::Queue m_graphics_queue;
		uint32_t  m_graphics_queue_family;

		vk::UniqueSurfaceKHR m_surface;

		sAllocatedImage_vulkan m_depth_image;
		sAllocatedImage_vulkan m_render_image;
		vk::Extent2D           m_render_extent;

		vk::UniqueSwapchainKHR             m_swapchain;
		std::vector< vk::Image >           m_swapchain_images;
		std::vector< vk::UniqueImageView > m_swapchain_image_views;
		vk::Format                         m_swapchain_format;
		vk::Extent2D                       m_swapchain_extent;

		vk::PhysicalDevice   m_physical_device;
		vk::UniqueDevice     m_logical_device;
		vma::UniqueAllocator memory_allocator;

		vk::UniqueDescriptorSetLayout m_vertex_scene_uniform_layout;

		vk::UniqueSampler m_sampler_linear;
		vk::UniqueSampler m_sampler_nearest;

		uint32_t                         m_frames_in_flight;
		uint32_t                         m_frame_number;
		std::vector< sFrameData_vulkan > m_frame_datas;

		sSubmitContext_vulkan m_submit_context;

		vk::UniqueDescriptorPool m_imgui_descriptor_pool;

		PFN_vkGetInstanceProcAddr m_get_instance_proc_addr;
		PFN_vkGetDeviceProcAddr   m_get_device_proc_addr;

		vk::UniqueDebugUtilsMessengerEXT m_debug_messenger;
	};
}

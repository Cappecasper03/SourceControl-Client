#pragma once

#include <glm/ext/matrix_float4x4.hpp>
#include <vk_mem_alloc.hpp>
#include <vulkan/vulkan.hpp>

#include <tracy/TracyVulkan.hpp>

#include "engine/rendering/vulkan/descriptor/sDescriptorAllocator_vulkan.h"

namespace df::vulkan
{
	struct sAllocatedBuffer_vulkan
	{
		vma::UniqueBuffer     buffer;
		vma::UniqueAllocation allocation;
	};

	struct sFrameData_vulkan
	{
		vk::UniqueCommandPool   command_pool;
		vk::UniqueCommandBuffer command_buffer;

		vk::UniqueSemaphore swapchain_semaphore;
		vk::UniqueSemaphore render_semaphore;
		vk::UniqueFence     render_fence;

		// TODO: Create a real solution for this
		sAllocatedBuffer_vulkan     vertex_scene_uniform_buffer_3d;
		sAllocatedBuffer_vulkan     vertex_scene_uniform_buffer_2d;
		sDescriptorAllocator_vulkan descriptors;

		TracyVkCtx tracy_context;
	};

	struct sAllocatedImage_vulkan
	{
		vma::UniqueImage      image;
		vk::UniqueImageView   image_view;
		vma::UniqueAllocation allocation;
		vk::Extent3D          extent;
		vk::Format            format;
	};

	struct sSubmitContext_vulkan
	{
		vk::UniqueFence         fence;
		vk::UniqueCommandPool   command_pool;
		vk::UniqueCommandBuffer command_buffer;

		TracyVkCtx tracy_context;
	};

	struct sVertexSceneUniforms_vulkan
	{
		glm::mat4 view_projection;
	};

};

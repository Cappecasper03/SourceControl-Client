#pragma once

#include <tracy/Tracy.hpp>

#include "engine/managers/assets/cCameraManager.h"
#include "engine/rendering/cRenderer.h"
#include "engine/rendering/vulkan/assets/cQuad_vulkan.h"
#include "engine/rendering/vulkan/assets/cTexture_vulkan.h"
#include "engine/rendering/vulkan/cDeferredRenderer_vulkan.h"
#include "engine/rendering/vulkan/cFramebuffer_vulkan.h"
#include "engine/rendering/vulkan/cRenderer_vulkan.h"
#include "engine/rendering/vulkan/descriptor/sDescriptorWriter_vulkan.h"
#include "engine/rendering/vulkan/pipeline/cPipeline_vulkan.h"

namespace df::vulkan::render_callback
{
	inline void defaultQuad( const cPipeline_vulkan* _pipeline, const cQuad_vulkan* _quad )
	{
		ZoneScoped;
		cRenderer_vulkan*  renderer   = reinterpret_cast< cRenderer_vulkan* >( cRenderer::getRenderInstance() );
		sFrameData_vulkan& frame_data = renderer->getCurrentFrame();
		TracyVkZone( frame_data.tracy_context, frame_data.command_buffer.get(), __FUNCTION__ );

		const vk::UniqueCommandBuffer& command_buffer = frame_data.command_buffer;
		const cCamera*                 camera         = cCameraManager::getInstance()->current;

		const sAllocatedBuffer_vulkan& vertex_scene_buffer = camera->type == cCamera::ePerspective ? frame_data.vertex_scene_uniform_buffer_3d
		                                                                                           : frame_data.vertex_scene_uniform_buffer_2d;

		const sVertexSceneUniforms_vulkan vertex_scene_uniforms{
			.view_projection = camera->view_projection,
		};

		void* data_dst = renderer->getMemoryAllocator().mapMemory( vertex_scene_buffer.allocation.get() ).value;
		std::memcpy( data_dst, &vertex_scene_uniforms, sizeof( vertex_scene_uniforms ) );
		renderer->getMemoryAllocator().unmapMemory( vertex_scene_buffer.allocation.get() );

		std::vector< vk::DescriptorSet > descriptor_sets;
		descriptor_sets.push_back( frame_data.descriptors.allocate( renderer->getVertexSceneUniformLayout() ) );

		sDescriptorWriter_vulkan writer_scene;
		writer_scene.writeBuffer( 0, vertex_scene_buffer.buffer.get(), sizeof( vertex_scene_uniforms ), 0, vk::DescriptorType::eUniformBuffer );
		writer_scene.updateSet( descriptor_sets.back() );

		writer_scene.clear();
		descriptor_sets.push_back( frame_data.descriptors.allocate( _quad->getTextureLayout() ) );
		writer_scene.writeImage( 0,
		                         reinterpret_cast< cTexture_vulkan* >( _quad->texture )->getImage().image_view.get(),
		                         renderer->getNearestSampler(),
		                         vk::ImageLayout::eShaderReadOnlyOptimal,
		                         vk::DescriptorType::eCombinedImageSampler );
		writer_scene.updateSet( descriptor_sets.back() );

		command_buffer->bindPipeline( vk::PipelineBindPoint::eGraphics, _pipeline->pipeline.get() );
		command_buffer->bindDescriptorSets( vk::PipelineBindPoint::eGraphics,
		                                    _pipeline->layout.get(),
		                                    0,
		                                    static_cast< uint32_t >( descriptor_sets.size() ),
		                                    descriptor_sets.data(),
		                                    0,
		                                    nullptr );

		const cQuad_vulkan::sPushConstants push_constants{
			.world_matrix = _quad->transform->world,
			.color        = _quad->color,
		};

		command_buffer->pushConstants( _pipeline->layout.get(),
		                               vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
		                               0,
		                               sizeof( push_constants ),
		                               &push_constants );

		renderer->setViewportScissor();

		const vk::Buffer         vertex_buffers[] = { _quad->vertex_buffer.buffer.get() };
		constexpr vk::DeviceSize offsets[]        = { 0 };
		command_buffer->bindVertexBuffers( 0, 1, vertex_buffers, offsets );

		command_buffer->bindIndexBuffer( _quad->index_buffer.buffer.get(), 0, vk::IndexType::eUint32 );

		command_buffer->drawIndexed( static_cast< uint32_t >( _quad->getIndices().size() ), 1, 0, 0, 0 );
	}

	inline void defaultQuadFinalDeferred( const cPipeline_vulkan* _pipeline, const cQuad_vulkan* _quad )
	{
		ZoneScoped;
		cDeferredRenderer_vulkan* renderer   = reinterpret_cast< cDeferredRenderer_vulkan* >( cRenderer::getRenderInstance() );
		sFrameData_vulkan&        frame_data = renderer->getCurrentFrame();
		TracyVkZone( frame_data.tracy_context, frame_data.command_buffer.get(), __FUNCTION__ );

		const cFramebuffer_vulkan*                   framebuffer        = reinterpret_cast< const cFramebuffer_vulkan* >( renderer->getDeferredFramebuffer() );
		const std::vector< sAllocatedImage_vulkan >& framebuffer_images = framebuffer->getCurrentFrameImages( renderer->getCurrentFrameIndex() );
		const vk::UniqueCommandBuffer&               command_buffer     = frame_data.command_buffer;
		const cCamera*                               camera             = cCameraManager::getInstance()->current;

		const sAllocatedBuffer_vulkan& vertex_scene_buffer = camera->type == cCamera::ePerspective ? frame_data.vertex_scene_uniform_buffer_3d
		                                                                                           : frame_data.vertex_scene_uniform_buffer_2d;

		const sVertexSceneUniforms_vulkan vertex_scene_uniforms{
			.view_projection = camera->view_projection,
		};

		void* data_dst = renderer->getMemoryAllocator().mapMemory( vertex_scene_buffer.allocation.get() ).value;
		std::memcpy( data_dst, &vertex_scene_uniforms, sizeof( vertex_scene_uniforms ) );
		renderer->getMemoryAllocator().unmapMemory( vertex_scene_buffer.allocation.get() );

		std::vector< vk::DescriptorSet > descriptor_sets;
		descriptor_sets.push_back( frame_data.descriptors.allocate( renderer->getVertexSceneUniformLayout() ) );

		sDescriptorWriter_vulkan writer_scene;
		writer_scene.writeBuffer( 0, vertex_scene_buffer.buffer.get(), sizeof( vertex_scene_uniforms ), 0, vk::DescriptorType::eUniformBuffer );
		writer_scene.updateSet( descriptor_sets.back() );

		writer_scene.clear();
		descriptor_sets.push_back( frame_data.descriptors.allocate( renderer->getTextureLayout() ) );
		for( size_t i = 0; i < framebuffer_images.size(); ++i )
		{
			writer_scene.writeImage( static_cast< uint32_t >( i ),
			                         framebuffer_images[ i ].image_view.get(),
			                         renderer->getNearestSampler(),
			                         vk::ImageLayout::eShaderReadOnlyOptimal,
			                         vk::DescriptorType::eCombinedImageSampler );
		}
		writer_scene.updateSet( descriptor_sets.back() );

		command_buffer->bindPipeline( vk::PipelineBindPoint::eGraphics, _pipeline->pipeline.get() );
		command_buffer->bindDescriptorSets( vk::PipelineBindPoint::eGraphics,
		                                    _pipeline->layout.get(),
		                                    0,
		                                    static_cast< uint32_t >( descriptor_sets.size() ),
		                                    descriptor_sets.data(),
		                                    0,
		                                    nullptr );

		const cDeferredRenderer_vulkan::sPushConstants push_constants{
			.world_matrix = _quad->transform->world,
		};

		command_buffer->pushConstants( _pipeline->layout.get(), vk::ShaderStageFlagBits::eVertex, 0, sizeof( push_constants ), &push_constants );

		renderer->setViewportScissor();

		const vk::Buffer         vertex_buffers[] = { _quad->vertex_buffer.buffer.get() };
		constexpr vk::DeviceSize offsets[]        = { 0 };
		command_buffer->bindVertexBuffers( 0, 1, vertex_buffers, offsets );

		command_buffer->bindIndexBuffer( _quad->index_buffer.buffer.get(), 0, vk::IndexType::eUint32 );

		command_buffer->drawIndexed( static_cast< uint32_t >( _quad->getIndices().size() ), 1, 0, 0, 0 );
	}
}

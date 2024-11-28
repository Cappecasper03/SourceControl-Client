#pragma once

#include <tracy/Tracy.hpp>

#include "engine/managers/assets/cCameraManager.h"
#include "engine/rendering/vulkan/assets/cMesh_vulkan.h"
#include "engine/rendering/vulkan/assets/cTexture_vulkan.h"
#include "engine/rendering/vulkan/cRenderer_vulkan.h"
#include "engine/rendering/vulkan/descriptor/sDescriptorWriter_vulkan.h"
#include "engine/rendering/vulkan/pipeline/cPipeline_vulkan.h"

namespace df::vulkan::render_callback
{
	inline void defaultMeshAmbient( const cPipeline_vulkan* _pipeline, const cMesh_vulkan* _mesh )
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
		descriptor_sets.push_back( frame_data.descriptors.allocate( _mesh->getTextureLayout() ) );
		writer_scene.writeImage( 0,
		                         reinterpret_cast< cTexture_vulkan* >( _mesh->getTextures().at( aiTextureType_DIFFUSE ) )->getImage().image_view.get(),
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

		const cMesh_vulkan::sPushConstants push_constants{
			.world_matrix = _mesh->transform->world,
		};

		command_buffer->pushConstants( _pipeline->layout.get(), vk::ShaderStageFlagBits::eVertex, 0, sizeof( push_constants ), &push_constants );

		renderer->setViewportScissor();

		const vk::Buffer         vertex_buffers[] = { _mesh->vertex_buffer.buffer.get() };
		constexpr vk::DeviceSize offsets[]        = { 0 };
		command_buffer->bindVertexBuffers( 0, 1, vertex_buffers, offsets );

		command_buffer->bindIndexBuffer( _mesh->index_buffer.buffer.get(), 0, vk::IndexType::eUint32 );

		command_buffer->drawIndexed( static_cast< uint32_t >( _mesh->getIndices().size() ), 1, 0, 0, 0 );
	}

	inline void defaultMesh( const cPipeline_vulkan* _pipeline, const cMesh_vulkan* _mesh )
	{
		ZoneScoped;

		const std::string_view name( _pipeline->getName() );

		if( name.find( "ambient" ) != std::string::npos )
			defaultMeshAmbient( _pipeline, _mesh );
	}

	inline void defaultMeshDeferred( const cPipeline_vulkan* _pipeline, const cMesh_vulkan* _mesh )
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
		descriptor_sets.push_back( frame_data.descriptors.allocate( _mesh->getTextureLayout() ) );
		writer_scene.writeImage( 0,
		                         reinterpret_cast< cTexture_vulkan* >( _mesh->getTextures().at( aiTextureType_DIFFUSE ) )->getImage().image_view.get(),
		                         renderer->getNearestSampler(),
		                         vk::ImageLayout::eShaderReadOnlyOptimal,
		                         vk::DescriptorType::eCombinedImageSampler );
		writer_scene.writeImage( 1,
		                         reinterpret_cast< cTexture_vulkan* >( _mesh->getTextures().at( aiTextureType_NORMALS ) )->getImage().image_view.get(),
		                         renderer->getNearestSampler(),
		                         vk::ImageLayout::eShaderReadOnlyOptimal,
		                         vk::DescriptorType::eCombinedImageSampler );
		writer_scene.writeImage( 2,
		                         reinterpret_cast< cTexture_vulkan* >( _mesh->getTextures().at( aiTextureType_SPECULAR ) )->getImage().image_view.get(),
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

		const cMesh_vulkan::sPushConstants push_constants{
			.world_matrix = _mesh->transform->world,
		};

		command_buffer->pushConstants( _pipeline->layout.get(), vk::ShaderStageFlagBits::eVertex, 0, sizeof( push_constants ), &push_constants );

		renderer->setViewportScissor();

		const vk::Buffer         vertex_buffers[] = { _mesh->vertex_buffer.buffer.get() };
		constexpr vk::DeviceSize offsets[]        = { 0 };
		command_buffer->bindVertexBuffers( 0, 1, vertex_buffers, offsets );

		command_buffer->bindIndexBuffer( _mesh->index_buffer.buffer.get(), 0, vk::IndexType::eUint32 );

		command_buffer->drawIndexed( static_cast< uint32_t >( _mesh->getIndices().size() ), 1, 0, 0, 0 );
	}
}

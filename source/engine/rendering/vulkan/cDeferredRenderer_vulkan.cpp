#include "cDeferredRenderer_vulkan.h"

#include <glm/ext/matrix_transform.hpp>
#include <tracy/Tracy.hpp>

#include "assets/cQuad_vulkan.h"
#include "callbacks/DefaultQuadCB_vulkan.h"
#include "cFramebuffer_vulkan.h"
#include "descriptor/sDescriptorLayoutBuilder_vulkan.h"
#include "engine/managers/cEventManager.h"
#include "engine/rendering/cRenderCallback.h"
#include "misc/Helper_vulkan.h"
#include "pipeline/sPipelineCreateInfo_vulkan.h"

namespace df::vulkan
{
	cDeferredRenderer_vulkan::cDeferredRenderer_vulkan( const std::string& _window_name )
		: cRenderer_vulkan( _window_name )
		, m_begin_deferred( true )
	{
		ZoneScoped;
	}

	cDeferredRenderer_vulkan::~cDeferredRenderer_vulkan()
	{
		ZoneScoped;

		delete m_deferred_framebuffer;
		delete m_deferred_screen_quad->render_callback;
		delete m_deferred_screen_quad;
		m_texture_layout.reset();
	}

	void cDeferredRenderer_vulkan::beginRendering( const int _clear_buffers, const cColor& _color )
	{
		if( !m_begin_deferred )
		{
			cRenderer_vulkan::beginRendering( _clear_buffers, _color );
			return;
		}

		ZoneScoped;
		const sFrameData_vulkan& frame_data = getCurrentFrame();
		TracyVkZone( frame_data.tracy_context, frame_data.command_buffer.get(), __FUNCTION__ );

		const bool color = _clear_buffers & cCamera::eClearBuffer::eColor;
		const bool depth = _clear_buffers & cCamera::eClearBuffer::eDepth;

		const vk::UniqueCommandBuffer& command_buffer = frame_data.command_buffer;
		const vk::ClearValue           clear_color_value( vk::ClearColorValue( _color.r, _color.g, _color.b, _color.a ) );
		constexpr vk::ClearValue       clear_depth_stencil_value( vk::ClearDepthStencilValue( 1 ) );

		const cFramebuffer_vulkan*                   framebuffer        = reinterpret_cast< cFramebuffer_vulkan* >( m_deferred_framebuffer );
		const std::vector< sAllocatedImage_vulkan >& framebuffer_images = framebuffer->getCurrentFrameImages( getCurrentFrameIndex() );

		std::vector< vk::RenderingAttachmentInfo > color_attachments;
		for( const sAllocatedImage_vulkan& framebuffer_image: framebuffer_images )
		{
			color_attachments.push_back(
				helper::init::attachmentInfo( framebuffer_image.image_view.get(), color ? &clear_color_value : nullptr, vk::ImageLayout::eColorAttachmentOptimal ) );
		}

		const vk::RenderingAttachmentInfo depth_attachment = helper::init::attachmentInfo( m_depth_image.image_view.get(),
		                                                                                   depth ? &clear_depth_stencil_value : nullptr,
		                                                                                   vk::ImageLayout::eDepthAttachmentOptimal );

		const vk::RenderingInfo rendering_info = helper::init::renderingInfo( m_render_extent, color_attachments, &depth_attachment );
		command_buffer->beginRendering( &rendering_info );
	}

	void cDeferredRenderer_vulkan::renderDeferred( const vk::CommandBuffer& _command_buffer )
	{
		ZoneScoped;
#ifdef PROFILING
		const sFrameData_vulkan& frame_data = getCurrentFrame();
		TracyVkZone( frame_data.tracy_context, frame_data.command_buffer.get(), __FUNCTION__ );
#endif

		const cFramebuffer_vulkan*                   framebuffer     = reinterpret_cast< cFramebuffer_vulkan* >( m_deferred_framebuffer );
		const std::vector< sAllocatedImage_vulkan >& deferred_images = framebuffer->getCurrentFrameImages( getCurrentFrameIndex() );

		for( const sAllocatedImage_vulkan& image: deferred_images )
			helper::util::transitionImage( _command_buffer, image.image.get(), vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral );

		m_begin_deferred = true;
		cEventManager::invoke( event::render_3d );
		cEventManager::invoke( event::render_2d );

		for( const sAllocatedImage_vulkan& image: deferred_images )
			helper::util::transitionImage( _command_buffer, image.image.get(), vk::ImageLayout::eUndefined, vk::ImageLayout::eShaderReadOnlyOptimal );

		m_begin_deferred = false;
		cCamera* camera  = cCameraManager::get( "default_2d" );
		camera->beginRender( cCamera::eDepth );
		m_deferred_screen_quad->render();
		camera->endRender();
	}

	void cDeferredRenderer_vulkan::initializeDeferred()
	{
		ZoneScoped;

		m_deferred_screen_quad = new cQuad_vulkan( "deferred", glm::vec3( m_window_size / 2, 0 ), glm::vec2( m_window_size ) );
		glm::mat4& transform   = m_deferred_screen_quad->transform->local;
		transform              = rotate( transform, glm::radians( 180.f ), glm::vec3( 0.f, 0.f, 1.f ) );
		transform              = rotate( transform, glm::radians( 180.f ), glm::vec3( 0.f, 1.f, 0.f ) );
		m_deferred_screen_quad->transform->update();

		createQuadRenderCallback();

		m_deferred_framebuffer = new cFramebuffer_vulkan( "deferred", 3, m_frames_in_flight, m_window_size );
	}

	void cDeferredRenderer_vulkan::createQuadRenderCallback()
	{
		ZoneScoped;

		sPipelineCreateInfo_vulkan pipeline_create_info{ .name = "default_quad_final_deferred" };

		pipeline_create_info.vertex_input_binding.emplace_back( 0, static_cast< uint32_t >( sizeof( cQuad_vulkan::sVertex ) ), vk::VertexInputRate::eVertex );

		pipeline_create_info.vertex_input_attribute.emplace_back( 0,
		                                                          0,
		                                                          vk::Format::eR32G32B32Sfloat,
		                                                          static_cast< uint32_t >( offsetof( cQuad_vulkan::sVertex, cQuad_vulkan::sVertex::position ) ) );
		pipeline_create_info.vertex_input_attribute.emplace_back( 1,
		                                                          0,
		                                                          vk::Format::eR32G32Sfloat,
		                                                          static_cast< uint32_t >( offsetof( cQuad_vulkan::sVertex, cQuad_vulkan::sVertex::tex_coord ) ) );

		pipeline_create_info.push_constant_ranges.emplace_back( vk::ShaderStageFlagBits::eVertex, 0, static_cast< uint32_t >( sizeof( sPushConstants ) ) );

		sDescriptorLayoutBuilder_vulkan descriptor_layout_builder{};
		descriptor_layout_builder.addBinding( 0, vk::DescriptorType::eCombinedImageSampler );
		descriptor_layout_builder.addBinding( 1, vk::DescriptorType::eCombinedImageSampler );
		descriptor_layout_builder.addBinding( 2, vk::DescriptorType::eCombinedImageSampler );
		m_texture_layout = descriptor_layout_builder.build( vk::ShaderStageFlagBits::eFragment );

		pipeline_create_info.descriptor_layouts.push_back( getVertexSceneUniformLayout() );
		pipeline_create_info.descriptor_layouts.push_back( m_texture_layout.get() );

		pipeline_create_info.setShaders( helper::util::createShaderModule( "default_quad_final_deferred.vert" ),
		                                 helper::util::createShaderModule( "default_quad_final_deferred.vert" ) );
		pipeline_create_info.setInputTopology( vk::PrimitiveTopology::eTriangleList );
		pipeline_create_info.setpolygonMode( vk::PolygonMode::eFill );
		pipeline_create_info.setCullMode( vk::CullModeFlagBits::eFront, vk::FrontFace::eClockwise );
		pipeline_create_info.setColorFormat( getRenderColorFormat() );
		pipeline_create_info.setDepthFormat( getRenderDepthFormat() );
		pipeline_create_info.setMultisamplingNone();
		pipeline_create_info.enableDepthtest( true, vk::CompareOp::eLessOrEqual );
		pipeline_create_info.disableBlending();

		m_deferred_screen_quad->render_callback = new cRenderCallback( "default_quad_final_deferred", pipeline_create_info, render_callback::defaultQuadFinalDeferred );
	}
}
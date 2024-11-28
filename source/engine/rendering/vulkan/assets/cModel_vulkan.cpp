#include "cModel_vulkan.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <ranges>
#include <tracy/Tracy.hpp>

#include "cMesh_vulkan.h"
#include "engine/filesystem/cFileSystem.h"
#include "engine/managers/cRenderCallbackManager.h"
#include "engine/rendering/cRenderer.h"
#include "engine/rendering/vulkan/callbacks/DefaultMeshCB_vulkan.h"
#include "engine/rendering/vulkan/cRenderer_vulkan.h"
#include "engine/rendering/vulkan/descriptor/sDescriptorLayoutBuilder_vulkan.h"
#include "engine/rendering/vulkan/misc/Helper_vulkan.h"
#include "engine/rendering/vulkan/pipeline/sPipelineCreateInfo_vulkan.h"

namespace df::vulkan
{

	cModel_vulkan::cModel_vulkan( std::string _name )
		: iModel( std::move( _name ) )
	{
		ZoneScoped;
	}

	iRenderCallback* cModel_vulkan::createDefaults()
	{
		ZoneScoped;

		if( cRenderer::isDeferred() )
			return createDefaultsDeferred();

		const cRenderer_vulkan* renderer = reinterpret_cast< cRenderer_vulkan* >( cRenderer::getRenderInstance() );

		sPipelineCreateInfo_vulkan pipeline_create_info{ .name = "default_mesh_ambient" };

		pipeline_create_info.vertex_input_binding.emplace_back( 0, static_cast< uint32_t >( sizeof( iMesh::sVertex ) ), vk::VertexInputRate::eVertex );

		pipeline_create_info.vertex_input_attribute.emplace_back( 0,
		                                                          0,
		                                                          vk::Format::eR32G32B32Sfloat,
		                                                          static_cast< uint32_t >( offsetof( iMesh::sVertex, iMesh::sVertex::position ) ) );
		pipeline_create_info.vertex_input_attribute.emplace_back( 1,
		                                                          0,
		                                                          vk::Format::eR32G32B32Sfloat,
		                                                          static_cast< uint32_t >( offsetof( iMesh::sVertex, iMesh::sVertex::normal ) ) );
		pipeline_create_info.vertex_input_attribute.emplace_back( 2,
		                                                          0,
		                                                          vk::Format::eR32G32B32Sfloat,
		                                                          static_cast< uint32_t >( offsetof( iMesh::sVertex, iMesh::sVertex::tangent ) ) );
		pipeline_create_info.vertex_input_attribute.emplace_back( 3,
		                                                          0,
		                                                          vk::Format::eR32G32B32Sfloat,
		                                                          static_cast< uint32_t >( offsetof( iMesh::sVertex, iMesh::sVertex::bitangent ) ) );
		pipeline_create_info.vertex_input_attribute.emplace_back( 4,
		                                                          0,
		                                                          vk::Format::eR32G32Sfloat,
		                                                          static_cast< uint32_t >( offsetof( iMesh::sVertex, iMesh::sVertex::tex_coords ) ) );

		pipeline_create_info.push_constant_ranges.emplace_back( vk::ShaderStageFlagBits::eVertex, 0, static_cast< uint32_t >( sizeof( cMesh_vulkan::sPushConstants ) ) );

		sDescriptorLayoutBuilder_vulkan descriptor_layout_builder{};
		descriptor_layout_builder.addBinding( 0, vk::DescriptorType::eCombinedImageSampler );
		cMesh_vulkan::s_texture_layout = descriptor_layout_builder.build( vk::ShaderStageFlagBits::eFragment );

		pipeline_create_info.descriptor_layouts.push_back( renderer->getVertexSceneUniformLayout() );
		pipeline_create_info.descriptor_layouts.push_back( cMesh_vulkan::s_texture_layout.get() );

		pipeline_create_info.setShaders( helper::util::createShaderModule( "default_mesh_ambient.vert" ), helper::util::createShaderModule( "default_mesh_ambient.frag" ) );
		pipeline_create_info.setInputTopology( vk::PrimitiveTopology::eTriangleList );
		pipeline_create_info.setpolygonMode( vk::PolygonMode::eFill );
		pipeline_create_info.setCullMode( vk::CullModeFlagBits::eNone, vk::FrontFace::eClockwise );
		pipeline_create_info.setColorFormat( renderer->getRenderColorFormat() );
		pipeline_create_info.setDepthFormat( renderer->getRenderDepthFormat() );
		pipeline_create_info.setMultisamplingNone();
		pipeline_create_info.enableDepthtest( true, vk::CompareOp::eLessOrEqual );
		pipeline_create_info.disableBlending();

		return cRenderCallbackManager::create( "default_mesh", pipeline_create_info, render_callback::defaultMesh );
	}

	void cModel_vulkan::destroyDefaults()
	{
		ZoneScoped;

		cMesh_vulkan::s_texture_layout.reset();
	}

	bool cModel_vulkan::processNode( const aiNode* _node, const aiScene* _scene )
	{
		ZoneScoped;

		if( !_node )
			return false;

		for( unsigned i = 0; i < _node->mNumMeshes; ++i )
			meshes.push_back( new cMesh_vulkan( _scene->mMeshes[ _node->mMeshes[ i ] ], _scene, this ) );

		for( unsigned i = 0; i < _node->mNumChildren; ++i )
			processNode( _node->mChildren[ i ], _scene );

		return true;
	}

	iRenderCallback* cModel_vulkan::createDefaultsDeferred()
	{
		ZoneScoped;

		const cRenderer_vulkan* renderer = reinterpret_cast< cRenderer_vulkan* >( cRenderer::getRenderInstance() );

		sPipelineCreateInfo_vulkan pipeline_create_info{ .name = "default_mesh_deferred" };

		pipeline_create_info.vertex_input_binding.emplace_back( 0, static_cast< uint32_t >( sizeof( iMesh::sVertex ) ), vk::VertexInputRate::eVertex );

		pipeline_create_info.vertex_input_attribute.emplace_back( 0,
		                                                          0,
		                                                          vk::Format::eR32G32B32Sfloat,
		                                                          static_cast< uint32_t >( offsetof( iMesh::sVertex, iMesh::sVertex::position ) ) );
		pipeline_create_info.vertex_input_attribute.emplace_back( 1,
		                                                          0,
		                                                          vk::Format::eR32G32B32Sfloat,
		                                                          static_cast< uint32_t >( offsetof( iMesh::sVertex, iMesh::sVertex::normal ) ) );
		pipeline_create_info.vertex_input_attribute.emplace_back( 2,
		                                                          0,
		                                                          vk::Format::eR32G32B32Sfloat,
		                                                          static_cast< uint32_t >( offsetof( iMesh::sVertex, iMesh::sVertex::tangent ) ) );
		pipeline_create_info.vertex_input_attribute.emplace_back( 3,
		                                                          0,
		                                                          vk::Format::eR32G32B32Sfloat,
		                                                          static_cast< uint32_t >( offsetof( iMesh::sVertex, iMesh::sVertex::bitangent ) ) );
		pipeline_create_info.vertex_input_attribute.emplace_back( 4,
		                                                          0,
		                                                          vk::Format::eR32G32Sfloat,
		                                                          static_cast< uint32_t >( offsetof( iMesh::sVertex, iMesh::sVertex::tex_coords ) ) );

		pipeline_create_info.push_constant_ranges.emplace_back( vk::ShaderStageFlagBits::eVertex, 0, static_cast< uint32_t >( sizeof( cMesh_vulkan::sPushConstants ) ) );

		sDescriptorLayoutBuilder_vulkan descriptor_layout_builder{};
		descriptor_layout_builder.addBinding( 0, vk::DescriptorType::eCombinedImageSampler );
		descriptor_layout_builder.addBinding( 1, vk::DescriptorType::eCombinedImageSampler );
		descriptor_layout_builder.addBinding( 2, vk::DescriptorType::eCombinedImageSampler );
		cMesh_vulkan::s_texture_layout = descriptor_layout_builder.build( vk::ShaderStageFlagBits::eFragment );

		pipeline_create_info.descriptor_layouts.push_back( renderer->getVertexSceneUniformLayout() );
		pipeline_create_info.descriptor_layouts.push_back( cMesh_vulkan::s_texture_layout.get() );

		pipeline_create_info.setShaders( helper::util::createShaderModule( "default_mesh_deferred.vert" ), helper::util::createShaderModule( "default_mesh_deferred.frag" ) );
		pipeline_create_info.setInputTopology( vk::PrimitiveTopology::eTriangleList );
		pipeline_create_info.setpolygonMode( vk::PolygonMode::eFill );
		pipeline_create_info.setCullMode( vk::CullModeFlagBits::eNone, vk::FrontFace::eClockwise );
		pipeline_create_info.setColorFormats( { vk::Format::eR32G32B32A32Sfloat, vk::Format::eR32G32B32A32Sfloat, vk::Format::eR32G32B32A32Sfloat } );
		pipeline_create_info.setDepthFormat( renderer->getRenderDepthFormat() );
		pipeline_create_info.setMultisamplingNone();
		pipeline_create_info.enableDepthtest( true, vk::CompareOp::eLessOrEqual );
		pipeline_create_info.disableBlending();

		return cRenderCallbackManager::create( "default_mesh", pipeline_create_info, render_callback::defaultMeshDeferred );
	}
}

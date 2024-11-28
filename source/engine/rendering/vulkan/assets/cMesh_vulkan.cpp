#include "cMesh_vulkan.h"

#include <assimp/material.h>
#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <filesystem>

#include "cModel_vulkan.h"
#include "cTexture_vulkan.h"
#include "engine/managers/assets/cModelManager.h"
#include "engine/managers/cRenderCallbackManager.h"
#include "engine/rendering/cRenderer.h"
#include "engine/rendering/vulkan/cRenderer_vulkan.h"
#include "engine/rendering/vulkan/misc/Helper_vulkan.h"
#include "engine/rendering/vulkan/pipeline/cPipeline_vulkan.h"

namespace df::vulkan
{
	vk::UniqueDescriptorSetLayout cMesh_vulkan::s_texture_layout = {};

	cMesh_vulkan::cMesh_vulkan( const aiMesh* _mesh, const aiScene* _scene, cModel_vulkan* _parent )
		: iMesh( _mesh, _scene, _parent )
	{
		ZoneScoped;

		cMesh_vulkan::createTextures( _mesh, _scene );

		const cRenderer_vulkan* renderer = reinterpret_cast< cRenderer_vulkan* >( cRenderer::getRenderInstance() );

		const size_t vertex_buffer_size = sizeof( *m_vertices.data() ) * m_vertices.size();
		const size_t index_buffer_size  = sizeof( *m_indices.data() ) * m_indices.size();

		vertex_buffer = helper::util::createBuffer( vertex_buffer_size,
		                                            vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
		                                            vma::MemoryUsage::eGpuOnly );
		index_buffer  = helper::util::createBuffer( index_buffer_size, vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst, vma::MemoryUsage::eGpuOnly );

		sAllocatedBuffer_vulkan staging_buffer = helper::util::createBuffer( vertex_buffer_size + index_buffer_size,
		                                                                     vk::BufferUsageFlagBits::eTransferSrc,
		                                                                     vma::MemoryUsage::eCpuOnly );

		void* data_dst = renderer->getMemoryAllocator().mapMemory( staging_buffer.allocation.get() ).value;
		std::memcpy( data_dst, m_vertices.data(), vertex_buffer_size );
		std::memcpy( static_cast< char* >( data_dst ) + vertex_buffer_size, m_indices.data(), index_buffer_size );
		renderer->getMemoryAllocator().unmapMemory( staging_buffer.allocation.get() );

		renderer->immediateSubmit(
			[ & ]( const vk::CommandBuffer _command_buffer )
			{
				const vk::BufferCopy vertex_copy( 0, 0, vertex_buffer_size );
				_command_buffer.copyBuffer( staging_buffer.buffer.get(), vertex_buffer.buffer.get(), 1, &vertex_copy );

				const vk::BufferCopy index_copy( vertex_buffer_size, 0, index_buffer_size );
				_command_buffer.copyBuffer( staging_buffer.buffer.get(), index_buffer.buffer.get(), 1, &index_copy );
			} );
	}

	void cMesh_vulkan::render()
	{
		ZoneScoped;

		if( cModelManager::getForcedRenderCallback() )
			cRenderCallbackManager::render< cPipeline_vulkan >( cModelManager::getForcedRenderCallback(), this );
		else if( render_callback )
			cRenderCallbackManager::render< cPipeline_vulkan >( render_callback, this );
		else
			cRenderCallbackManager::render< cPipeline_vulkan >( cModelManager::getDefaultRenderCallback(), this );
	}

	void cMesh_vulkan::createTextures( const aiMesh* _mesh, const aiScene* _scene )
	{
		ZoneScoped;

		const aiMaterial* material = _scene->mMaterials[ _mesh->mMaterialIndex ];

		for( const aiTextureType& texture_type: { aiTextureType_DIFFUSE, aiTextureType_SPECULAR, aiTextureType_NORMALS } )
		{
			for( unsigned i = 0; i < material->GetTextureCount( texture_type ); ++i )
			{
				aiString path;
				material->GetTexture( texture_type, i, &path );

				const std::string texture_name = std::filesystem::path( path.data ).filename().replace_extension().string();
				const std::string full_path    = fmt::format( "{}/{}", m_parent->folder, path.data );

				if( auto it = m_parent->textures.find( full_path ); it != m_parent->textures.end() && it->second )
				{
					m_textures[ texture_type ] = it->second;
					continue;
				}

				cTexture_vulkan* texture = new cTexture_vulkan( texture_name );
				if( !texture->load( full_path ) )
				{
					delete texture;
					continue;
				}

				m_textures[ texture_type ]      = texture;
				m_parent->textures[ full_path ] = texture;
			}

			if( m_textures.contains( texture_type ) )
				continue;

			if( auto it = m_parent->textures.find( "white" ); it != m_parent->textures.end() && it->second )
			{
				m_textures[ texture_type ] = it->second;
				continue;
			}

			cTexture_vulkan* texture      = new cTexture_vulkan( "white" );
			m_textures[ texture_type ]    = texture;
			m_parent->textures[ "white" ] = texture;
		}
	}
}

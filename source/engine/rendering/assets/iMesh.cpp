#include "iMesh.h"

#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <filesystem>

#include "engine/managers/cRenderCallbackManager.h"
#include "iModel.h"

namespace df
{
	iMesh::iMesh( const aiMesh* _mesh, const aiScene* /*_scene*/, iModel* _parent )
		: iRenderAsset( _mesh->mName.data )
		, m_parent( _parent )
	{
		ZoneScoped;

		m_parent->transform->addChild( *transform );
		createVertices( _mesh );
		createIndices( _mesh );
	}

	void iMesh::createVertices( const aiMesh* _mesh )
	{
		ZoneScoped;

		m_vertices.reserve( _mesh->mNumVertices );
		for( unsigned i = 0; i < _mesh->mNumVertices; ++i )
		{
			sVertex vertex{};

			if( _mesh->mVertices )
			{
				const aiVector3D& ai_vertex = _mesh->mVertices[ i ];
				vertex.position             = { ai_vertex.x, ai_vertex.y, ai_vertex.z };
			}

			if( _mesh->mNormals )
			{
				const aiVector3D& ai_normal = _mesh->mNormals[ i ];
				vertex.normal               = { ai_normal.x, ai_normal.y, ai_normal.z };
			}

			if( _mesh->mTangents )
			{
				const aiVector3D& ai_tangent = _mesh->mTangents[ i ];
				vertex.tangent               = { ai_tangent.x, ai_tangent.y, ai_tangent.z };
			}

			if( _mesh->mBitangents )
			{
				const aiVector3D& ai_bitangent = _mesh->mBitangents[ i ];
				vertex.bitangent               = { ai_bitangent.x, ai_bitangent.y, ai_bitangent.z };
			}

			if( _mesh->mTextureCoords[ 0 ] )
			{
				const aiVector3D& ai_texture_coords = _mesh->mTextureCoords[ 0 ][ i ];
				vertex.tex_coords                   = { ai_texture_coords.x, ai_texture_coords.y };
			}

			m_vertices.push_back( vertex );
		}
	}

	void iMesh::createIndices( const aiMesh* _mesh )
	{
		ZoneScoped;

		for( unsigned i = 0; i < _mesh->mNumFaces; ++i )
		{
			const aiFace& face = _mesh->mFaces[ i ];
			m_indices.reserve( m_indices.size() + face.mNumIndices );

			for( unsigned j = 0; j < face.mNumIndices; ++j )
				m_indices.push_back( face.mIndices[ j ] );
		}
	}
}

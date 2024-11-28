#pragma once

#include <assimp/material.h>
#include <unordered_map>

#include "AssetTypes.h"

struct aiScene;
struct aiMesh;

namespace df
{
	class iTexture;
	class iModel;

	class iMesh : public iRenderAsset
	{
	public:
		DF_DISABLE_COPY_AND_MOVE( iMesh )

		struct sVertex
		{
			glm::vec3 position   = glm::vec3( 0 );
			glm::vec3 normal     = glm::vec3( 0 );
			glm::vec3 tangent    = glm::vec3( 0 );
			glm::vec3 bitangent  = glm::vec3( 0 );
			glm::vec2 tex_coords = glm::vec2( 0 );
		};

		explicit iMesh( const aiMesh* _mesh, const aiScene* _scene, iModel* _parent );
		~iMesh() override = default;

		const std::vector< sVertex >&                         getVertices() const { return m_vertices; }
		const std::vector< unsigned >&                        getIndices() const { return m_indices; }
		const std::unordered_map< aiTextureType, iTexture* >& getTextures() const { return m_textures; }

	protected:
		void         createVertices( const aiMesh* _mesh );
		void         createIndices( const aiMesh* _mesh );
		virtual void createTextures( const aiMesh* _mesh, const aiScene* _scene ) = 0;

		std::vector< sVertex >                         m_vertices;
		std::vector< unsigned >                        m_indices;
		std::unordered_map< aiTextureType, iTexture* > m_textures;

		iModel* m_parent;
	};
}

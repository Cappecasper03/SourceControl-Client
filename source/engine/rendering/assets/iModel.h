#pragma once

#include <assimp/postprocess.h>
#include <string>
#include <unordered_map>

#include "AssetTypes.h"
#include "engine/misc/Misc.h"

struct aiMesh;
struct aiScene;
struct aiNode;

namespace df
{
	class iMesh;
	class iTexture;

	class iModel : public iRenderAsset
	{
	public:
		DF_DISABLE_COPY_AND_MOVE( iModel )

		explicit iModel( std::string _name );
		~iModel() override;

		void render() override;

		bool load( const std::string& _folder_path, unsigned _load_flags = aiProcess_Triangulate | aiProcess_JoinIdenticalVertices );

		std::vector< iMesh* >                        meshes;
		std::string                                  folder;
		std::unordered_map< std::string, iTexture* > textures;

	protected:
		virtual bool processNode( const aiNode* _node, const aiScene* _scene ) = 0;
	};
}

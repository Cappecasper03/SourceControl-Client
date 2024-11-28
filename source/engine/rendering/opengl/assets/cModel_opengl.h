#pragma once

#include <string>

#include "engine/misc/Misc.h"
#include "engine/rendering/assets/iModel.h"
#include "sRenderAsset_opengl.h"

struct aiMesh;
struct aiScene;
struct aiNode;

namespace df::opengl
{
	class cMesh_opengl;
	class cTexture_opengl;

	class cModel_opengl : public sRenderAsset_opengl,
						  public iModel
	{
	public:
		DF_DISABLE_COPY_AND_MOVE( cModel_opengl )

		friend cMesh_opengl;

		explicit cModel_opengl( std::string _name );
		~cModel_opengl() override = default;

		static iRenderCallback* createDefaults();

	private:
		bool processNode( const aiNode* _node, const aiScene* _scene ) override;
	};
}

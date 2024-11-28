#pragma once

#include "engine/rendering/assets/iMesh.h"
#include "sRenderAsset_opengl.h"

struct aiScene;
struct aiMesh;

namespace df::opengl
{
	class cTexture_opengl;
	class cModel_opengl;

	class cMesh_opengl : public sRenderAsset_opengl,
						 public iMesh
	{
	public:
		DF_DISABLE_COPY_AND_MOVE( cMesh_opengl )

		explicit cMesh_opengl( const aiMesh* _mesh, const aiScene* _scene, cModel_opengl* _parent );
		~cMesh_opengl() override = default;

		void render() override;

	private:
		void createTextures( const aiMesh* _mesh, const aiScene* _scene ) override;
	};
}

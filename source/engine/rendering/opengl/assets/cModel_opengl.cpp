#include "cModel_opengl.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <ranges>
#include <tracy/Tracy.hpp>

#include "cMesh_opengl.h"
#include "engine/filesystem/cFileSystem.h"
#include "engine/managers/cRenderCallbackManager.h"
#include "engine/rendering/cRenderer.h"
#include "engine/rendering/opengl/callbacks/DefaultMeshCB_opengl.h"

namespace df::opengl
{
	cModel_opengl::cModel_opengl( std::string _name )
		: iModel( std::move( _name ) )
	{}

	iRenderCallback* cModel_opengl::createDefaults()
	{
		ZoneScoped;

		iRenderCallback* callback;

		if( cRenderer::isDeferred() )
			callback = cRenderCallbackManager::create( "default_mesh_deferred", render_callback::defaultMeshDeferred );
		else
		{
			const std::vector< std::string > shader_names = { "default_mesh_ambient" };
			callback                                      = cRenderCallbackManager::create( "default_mesh", shader_names, render_callback::defaultMesh );
		}

		return callback;
	}

	bool cModel_opengl::processNode( const aiNode* _node, const aiScene* _scene )
	{
		ZoneScoped;

		if( !_node )
			return false;

		for( unsigned i = 0; i < _node->mNumMeshes; ++i )
			meshes.push_back( new cMesh_opengl( _scene->mMeshes[ _node->mMeshes[ i ] ], _scene, this ) );

		for( unsigned i = 0; i < _node->mNumChildren; ++i )
			processNode( _node->mChildren[ i ], _scene );

		return true;
	}
}

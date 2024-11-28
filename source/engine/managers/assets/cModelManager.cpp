#include "cModelManager.h"

#include "engine/rendering/cRenderer.h"
#include "engine/rendering/opengl/assets/cModel_opengl.h"
#include "engine/rendering/vulkan/assets/cModel_vulkan.h"

namespace df
{
	cModelManager::cModelManager()
	{
		ZoneScoped;

		switch( cRenderer::getInstanceType() )
		{
			case cRenderer::eOpenGL:
			{
				m_default_render_callback = opengl::cModel_opengl::createDefaults();
				break;
			}
			case cRenderer::eVulkan:
			{
				m_default_render_callback = vulkan::cModel_vulkan::createDefaults();
				break;
			}
		}
	}

	cModelManager::~cModelManager()
	{
		ZoneScoped;

		switch( cRenderer::getInstanceType() )
		{
			case cRenderer::eOpenGL:
				break;
			case cRenderer::eVulkan:
			{
				vulkan::cModel_vulkan::destroyDefaults();
				break;
			}
		}
	}

	iModel* cModelManager::load( const std::string& _name, const std::string& _folder_path, const unsigned _load_flags )
	{
		ZoneScoped;

		iModel* model = nullptr;
		switch( cRenderer::getInstanceType() )
		{
			case cRenderer::eOpenGL:
			{
				model = create< opengl::cModel_opengl >( _name );
				break;
			}
			case cRenderer::eVulkan:
			{
				model = create< vulkan::cModel_vulkan >( _name );
				break;
			}
		}

		if( model )
			model->load( _folder_path, _load_flags );

		return model;
	}
}

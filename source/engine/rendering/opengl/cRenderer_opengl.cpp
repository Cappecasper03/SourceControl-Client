#include "cRenderer_opengl.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <tracy/TracyOpenGL.hpp>

#include "assets/cTexture_opengl.h"
#include "callbacks/DefaultQuadCB_opengl.h"
#include "cFramebuffer_opengl.h"
#include "engine/filesystem/cFileSystem.h"
#include "engine/managers/cEventManager.h"
#include "engine/managers/cRenderCallbackManager.h"

namespace df::opengl
{
	cRenderer_opengl::cRenderer_opengl( const std::string& _window_name )
	{
		ZoneScoped;

		glfwInit();
		DF_LOG_MESSAGE( "Initialized GLFW" );

		glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
		glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 6 );
		glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

		m_window = glfwCreateWindow( m_window_size.x, m_window_size.y, _window_name.data(), nullptr, nullptr );
		if( !m_window )
		{
			DF_LOG_ERROR( "Failed to create window" );
			return;
		}
		DF_LOG_MESSAGE( fmt::format( "Created window [{}, {}]", m_window_size.x, m_window_size.y ) );

		glfwMakeContextCurrent( m_window );
		glfwSwapInterval( 0 );

		if( !gladLoadGLLoader( reinterpret_cast< GLADloadproc >( glfwGetProcAddress ) ) )
		{
			DF_LOG_ERROR( "Failed to initialize GLAD" );
			return;
		}
		DF_LOG_MESSAGE( "Initialized GLAD" );

		TracyGpuContext;
		glViewport( 0, 0, m_window_size.x, m_window_size.y );

		glfwSetFramebufferSizeCallback( m_window, framebufferSizeCallback );

		if( cRenderer::isDeferred() )
			initializeDeferred();

#ifdef DEBUG
		glEnable( GL_DEBUG_OUTPUT );
		glDebugMessageCallback( debugMessageCallback, nullptr );
#endif
	}

	cRenderer_opengl::~cRenderer_opengl()
	{
		ZoneScoped;
		TracyGpuZone( __FUNCTION__ );

		if( ImGui::GetCurrentContext() )
		{
			ImGui_ImplGlfw_Shutdown();
			ImGui_ImplOpenGL3_Shutdown();
			ImGui::DestroyContext();
		}

		if( cRenderer::isDeferred() )
		{
			delete m_deferred_framebuffer;
			delete m_deferred_screen_quad->render_callback;
			delete m_deferred_screen_quad;
		}

		if( m_window )
			glfwDestroyWindow( m_window );

		glfwTerminate();
		DF_LOG_MESSAGE( "Deinitialized GLFW" );
	}

	void cRenderer_opengl::render()
	{
		ZoneScoped;
		TracyGpuZone( __FUNCTION__ );

		if( cRenderer::isDeferred() )
		{
			m_deferred_framebuffer->bind();

			cEventManager::invoke( event::render_3d );
			cEventManager::invoke( event::render_2d );

			m_deferred_framebuffer->unbind();

			cCamera* camera = cCameraManager::get( "default_2d" );
			camera->beginRender( cCamera::eDepth );

			m_deferred_screen_quad->render();

			camera->endRender();
		}
		else
		{
			cEventManager::invoke( event::render_3d );
			cEventManager::invoke( event::render_2d );
		}

		if( ImGui::GetCurrentContext() )
		{
			TracyGpuNamedZone( imgui, __FUNCTION__ "::ImGui", true );
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			cEventManager::invoke( event::imgui );
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
		}

		glfwSwapBuffers( m_window );
		TracyGpuCollect;
	}

	void cRenderer_opengl::beginRendering( const int _clear_buffers, const cColor& _color )
	{
		ZoneScoped;
		TracyGpuZone( __FUNCTION__ );

		const int color = _clear_buffers & cCamera::eClearBuffer::eColor ? GL_COLOR_BUFFER_BIT : 0;
		const int depth = _clear_buffers & cCamera::eClearBuffer::eDepth ? GL_DEPTH_BUFFER_BIT : 0;

		glClearColor( _color.r, _color.g, _color.b, _color.a );
		glClear( color | depth );
	}

	void cRenderer_opengl::initializeImGui()
	{
		ZoneScoped;
		TracyGpuZone( __FUNCTION__ );

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io     = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

		ImGui_ImplGlfw_InitForOpenGL( m_window, true );
		ImGui_ImplOpenGL3_Init( "#version 460" );
	}

	void cRenderer_opengl::initializeDeferred()
	{
		ZoneScoped;
		TracyGpuZone( __FUNCTION__ );

		m_deferred_screen_quad                  = new cQuad_opengl( "deferred", glm::vec3( m_window_size / 2, 0 ), glm::vec2( m_window_size ) );
		m_deferred_screen_quad->render_callback = new cRenderCallback( "default_quad_deferred", "default_quad_deferred", render_callback::defaultQuadDeferred );

		m_deferred_framebuffer = new cFramebuffer_opengl( "deferred", 3, true, m_window_size );

		cTexture_opengl* texture = reinterpret_cast< cTexture_opengl* >( m_deferred_framebuffer->render_textues[ 0 ] );
		texture->bind();
		texture->setTexImage2D( 0, GL_RGB16F, m_window_size.x, m_window_size.y, 0, GL_RGB, GL_FLOAT, nullptr );

		texture = reinterpret_cast< cTexture_opengl* >( m_deferred_framebuffer->render_textues[ 1 ] );
		texture->bind();
		texture->setTexImage2D( 0, GL_RGB, m_window_size.x, m_window_size.y, 0, GL_RGB, GL_FLOAT, nullptr );

		texture = reinterpret_cast< cTexture_opengl* >( m_deferred_framebuffer->render_textues[ 2 ] );
		texture->bind();
		texture->setTexImage2D( 0, GL_RGBA, m_window_size.x, m_window_size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr );

		texture->unbind();
	}

	void cRenderer_opengl::framebufferSizeCallback( GLFWwindow* /*_window*/, const int _width, const int _height )
	{
		ZoneScoped;

		glViewport( 0, 0, _width, _height );

		if( _width == 0 || _height == 0 )
			return;

		cEventManager::invoke( event::on_window_resize, _width, _height );
	}

	void cRenderer_opengl::debugMessageCallback( unsigned _source,
	                                             unsigned _type,
	                                             unsigned _id,
	                                             unsigned _severity,
	                                             int /*_length*/,
	                                             const char* _message,
	                                             const void* /*_user_param*/ )
	{
		ZoneScoped;

		std::string source;
		switch( _source )
		{
			case GL_DEBUG_SOURCE_API:
			{
				source = "API";
			}
			break;
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
			{
				source = "Window System";
			}
			break;
			case GL_DEBUG_SOURCE_SHADER_COMPILER:
			{
				source = "Shader Compiler";
			}
			break;
			case GL_DEBUG_SOURCE_THIRD_PARTY:
			{
				source = "Third Party";
			}
			break;
			case GL_DEBUG_SOURCE_APPLICATION:
			{
				source = "Application";
			}
			break;
			case GL_DEBUG_SOURCE_OTHER:
			{
				source = "Other";
			}
			break;
			default:
			{
			}
		}

		std::string type;
		switch( _type )
		{
			case GL_DEBUG_TYPE_ERROR:
			{
				type = "Error";
			}
			break;
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			{
				type = "Deprecated Behavior";
			}
			break;
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			{
				type = "Undefined Behavior";
			}
			break;
			case GL_DEBUG_TYPE_PORTABILITY:
			{
				type = "Portability";
			}
			break;
			case GL_DEBUG_TYPE_PERFORMANCE:
			{
				type = "Performance";
			}
			break;
			case GL_DEBUG_TYPE_MARKER:
			{
				type = "Marker";
			}
			break;
			case GL_DEBUG_TYPE_PUSH_GROUP:
			{
				type = "Push Group";
			}
			break;
			case GL_DEBUG_TYPE_POP_GROUP:
			{
				type = "Pop Group";
			}
			break;
			case GL_DEBUG_TYPE_OTHER:
			{
				type = "Other";
			}
			break;
			default:
			{
			}
		}

		switch( _severity )
		{
			case GL_DEBUG_SEVERITY_HIGH:
			{
				DF_LOG_ERROR( fmt::format( "OpenGL, "
				                           "Source: {}, "
				                           "Type: {}, "
				                           "ID: {}, "
				                           "Severity: High, "
				                           "Message: {}",
				                           source,
				                           type,
				                           _id,
				                           _message ) );
			}
			break;
			case GL_DEBUG_SEVERITY_MEDIUM:
			{
				DF_LOG_WARNING( fmt::format( "OpenGL, "
				                             "Source: {}, "
				                             "Type: {}, "
				                             "ID: {}, "
				                             "Severity: Medium, "
				                             "Message: {}",
				                             source,
				                             type,
				                             _id,
				                             _message ) );
			}
			break;
			case GL_DEBUG_SEVERITY_LOW:
			{
				DF_LOG_WARNING( fmt::format( "OpenGL, "
				                             "Source: {}, "
				                             "Type: {}, "
				                             "ID: {}, "
				                             "Severity: Low, "
				                             "Message: {}",
				                             source,
				                             type,
				                             _id,
				                             _message ) );
			}
			break;
			default:
			{
			}
		}
	}
}

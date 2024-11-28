#pragma once

#include <fmt/format.h>

#include "engine/misc/iSingleton.h"
#include "engine/rendering/cRenderCallback.h"

namespace df
{
	struct iRenderCallback;

	class cRenderCallbackManager : public iSingleton< cRenderCallbackManager >
	{
	public:
		DF_DISABLE_COPY_AND_MOVE( cRenderCallbackManager );

		cRenderCallbackManager() = default;
		~cRenderCallbackManager() override { clear(); }

		template< typename T, typename... Targs >
		static cRenderCallback< T, Targs... >* create( const std::string& _shader_name, void( _callback )( const T*, Targs... ) );
		template< typename T, typename... Targs >
		static cRenderCallback< T, Targs... >* create( const std::string&                _callback_name,
		                                               const std::vector< std::string >& _shader_names,
		                                               void( _callback )( const T*, Targs... ) );

		template< typename T, typename... Targs >
		static cRenderCallback< T, Targs... >* create( const std::string& _name, const vulkan::sPipelineCreateInfo_vulkan& _pipelines, void( _callback )( const T*, Targs... ) );
		template< typename T, typename... Targs >
		static cRenderCallback< T, Targs... >* create( const std::string&                                       _name,
		                                               const std::vector< vulkan::sPipelineCreateInfo_vulkan >& _pipelines,
		                                               void( _callback )( const T*, Targs... ) );

		static bool destroy( const std::string& _name );
		static bool destroy( const iRenderCallback* _callback );
		static void clear();

		template< typename T, typename... Targs >
		static void render( const std::string& _name, Targs... _args );
		template< typename T, typename... Targs >
		static void render( iRenderCallback* _callback, Targs... _args );

		template< typename... Targs >
		static cRenderCallback< Targs... >* get( const std::string& _name )
		{
			return get( _name );
		}

		static iRenderCallback* get( const std::string& _name );

	private:
		std::unordered_map< std::string, iRenderCallback* > m_render_callbacks;
	};

	template< typename T, typename... Targs >
	cRenderCallback< T, Targs... >* cRenderCallbackManager::create( const std::string& _shader_name, void _callback( const T*, Targs... ) )
	{
		ZoneScoped;

		std::unordered_map< std::string, iRenderCallback* >& render_callbacks = getInstance()->m_render_callbacks;

		if( render_callbacks.contains( _shader_name ) )
		{
			DF_LOG_WARNING( fmt::format( "Callback already exist: {}", _shader_name ) );
			return nullptr;
		}

		cRenderCallback< T, Targs... >* callback = new cRenderCallback< T, Targs... >( _shader_name, _shader_name, _callback );
		render_callbacks[ _shader_name ]         = callback;

		DF_LOG_MESSAGE( fmt::format( "Created callback: {}", _shader_name ) );
		return callback;
	}

	template< typename T, typename... Targs >
	cRenderCallback< T, Targs... >* cRenderCallbackManager::create( const std::string&                _callback_name,
	                                                                const std::vector< std::string >& _shader_names,
	                                                                void                              _callback( const T*, Targs... ) )
	{
		ZoneScoped;

		std::unordered_map< std::string, iRenderCallback* >& render_callbacks = getInstance()->m_render_callbacks;

		if( render_callbacks.contains( _callback_name ) )
		{
			DF_LOG_WARNING( fmt::format( "Callback already exist: {}", _callback_name ) );
			return nullptr;
		}

		cRenderCallback< T, Targs... >* callback = new cRenderCallback< T, Targs... >( _callback_name, _shader_names, _callback );
		render_callbacks[ _callback_name ]       = callback;

		DF_LOG_MESSAGE( fmt::format( "Created callback: {}", _callback_name ) );
		return callback;
	}

	template< typename T, typename... Targs >
	cRenderCallback< T, Targs... >* cRenderCallbackManager::create( const std::string&                        _name,
	                                                                const vulkan::sPipelineCreateInfo_vulkan& _pipelines,
	                                                                void                                      _callback( const T*, Targs... ) )
	{
		ZoneScoped;

		std::unordered_map< std::string, iRenderCallback* >& render_callbacks = getInstance()->m_render_callbacks;

		if( render_callbacks.contains( _name ) )
		{
			DF_LOG_WARNING( fmt::format( "Callback already exist: {}", _name ) );
			return nullptr;
		}

		cRenderCallback< T, Targs... >* callback = new cRenderCallback< T, Targs... >( _name, _pipelines, _callback );
		render_callbacks[ _name ]                = callback;

		DF_LOG_MESSAGE( fmt::format( "Created callback: {}", _name ) );
		return callback;
	}

	template< typename T, typename... Targs >
	cRenderCallback< T, Targs... >* cRenderCallbackManager::create( const std::string&                                       _name,
	                                                                const std::vector< vulkan::sPipelineCreateInfo_vulkan >& _pipelines,
	                                                                void                                                     _callback( const T*, Targs... ) )
	{
		ZoneScoped;

		std::unordered_map< std::string, iRenderCallback* >& render_callbacks = getInstance()->m_render_callbacks;

		if( render_callbacks.contains( _name ) )
		{
			DF_LOG_WARNING( fmt::format( "Callback already exist: {}", _name ) );
			return nullptr;
		}

		cRenderCallback< T, Targs... >* callback = new cRenderCallback< T, Targs... >( _name, _pipelines, _callback );
		render_callbacks[ _name ]                = callback;

		DF_LOG_MESSAGE( fmt::format( "Created callback: {}", _name ) );
		return callback;
	}

	inline bool cRenderCallbackManager::destroy( const std::string& _name )
	{
		ZoneScoped;

		std::unordered_map< std::string, iRenderCallback* >& render_callbacks = getInstance()->m_render_callbacks;

		const auto it = render_callbacks.find( _name );
		if( it == render_callbacks.end() )
		{
			DF_LOG_WARNING( fmt::format( "Callback doesn't exist: {}", _name ) );
			return false;
		}

		delete it->second;
		render_callbacks.erase( it );
		DF_LOG_MESSAGE( fmt::format( "Destroyed callback: {}", _name ) );

		return true;
	}

	inline bool cRenderCallbackManager::destroy( const iRenderCallback* _callback )
	{
		ZoneScoped;

		if( !_callback )
			return false;

		std::unordered_map< std::string, iRenderCallback* >& render_callbacks = getInstance()->m_render_callbacks;

		for( const std::pair< const std::string, iRenderCallback* >& callback: render_callbacks )
		{
			if( callback.second == _callback )
			{
				DF_LOG_MESSAGE( fmt::format( "Destroyed callback: {}", callback.first ) );
				delete callback.second;
				render_callbacks.erase( callback.first );
				return true;
			}
		}

		DF_LOG_WARNING( fmt::format( "Callback isn't managed: {}", _callback->name ) );
		return false;
	}

	inline void cRenderCallbackManager::clear()
	{
		ZoneScoped;

		std::unordered_map< std::string, iRenderCallback* >& render_callbacks = getInstance()->m_render_callbacks;

		for( const std::pair< const std::string, iRenderCallback* >& callback: render_callbacks )
		{
			if( callback.second )
			{
				DF_LOG_MESSAGE( fmt::format( "Destroyed callback: {}", callback.first ) );
				delete callback.second;
			}
		}

		render_callbacks.clear();
	}

	template< typename T, typename... Targs >
	void cRenderCallbackManager::render( const std::string& _name, Targs... _args )
	{
		ZoneScoped;

		cRenderCallback< T, Targs... >* callback = reinterpret_cast< cRenderCallback< T, Targs... >* >( getInstance()->m_render_callbacks[ _name ] );
		if( callback )
			callback->render( _args... );
	}

	template< typename T, typename... Targs >
	void cRenderCallbackManager::render( iRenderCallback* _callback, Targs... _args )
	{
		ZoneScoped;

		cRenderCallback< T, Targs... >* callback = reinterpret_cast< cRenderCallback< T, Targs... >* >( _callback );
		if( callback )
			callback->render( _args... );
	}

	inline iRenderCallback* cRenderCallbackManager::get( const std::string& _name )
	{
		ZoneScoped;

		std::unordered_map< std::string, iRenderCallback* >& render_callbacks = getInstance()->m_render_callbacks;

		const auto it = render_callbacks.find( _name );
		if( it == render_callbacks.end() )
		{
			DF_LOG_WARNING( fmt::format( "Callback doesn't exist: {}", _name ) );
			return nullptr;
		}

		return it->second;
	}
}

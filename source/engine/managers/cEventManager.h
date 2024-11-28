#pragma once

#include <ranges>
#include <string>
#include <unordered_map>

#include "engine/events/cEvent.h"
#include "engine/misc/iSingleton.h"

namespace df
{
	namespace event
	{
		static std::string input            = "input";
		static std::string update           = "update";
		static std::string render_3d        = "render_3d";
		static std::string render_2d        = "render_2d";
		static std::string imgui            = "imgui";
		static std::string on_window_resize = "on_window_resize";
	}

	class cEventManager final : public iSingleton< cEventManager >
	{
	public:
		DF_DISABLE_COPY_AND_MOVE( cEventManager );

		cEventManager() = default;
		~cEventManager() override;

		template< typename T, typename... Targs >
		static void subscribe( const std::string& _name, T* _object, void ( T::*_function )( Targs... ) );
		template< typename T, typename... Targs >
		static void subscribe( const std::string& _name, T* _object, void ( *_function )( Targs... ) );

		template< typename T >
		static void unsubscribe( const std::string& _name, T* _object );

		template< typename... Targs >
		static void invoke( const std::string& _name, Targs... _args );

	private:
		std::unordered_map< std::string, iEvent* > m_events;
	};

	inline cEventManager::~cEventManager()
	{
		ZoneScoped;

		for( const iEvent* event: m_events | std::ranges::views::values )
			delete event;
	}

	template< typename T, typename... Targs >
	void cEventManager::subscribe( const std::string& _name, T* _object, void ( T::*_function )( Targs... ) )
	{
		ZoneScoped;

		auto event = reinterpret_cast< cEvent< Targs... >* >( getInstance()->m_events[ _name ] );

		if( !event )
		{
			event                            = new cEvent< Targs... >;
			getInstance()->m_events[ _name ] = event;
		}

		event->subscribe( _object, _function );
	}

	template< typename T, typename... Targs >
	void cEventManager::subscribe( const std::string& _name, T* _object, void ( *_function )( Targs... ) )
	{
		ZoneScoped;

		auto event = reinterpret_cast< cEvent< Targs... >* >( getInstance()->m_events[ _name ] );

		if( !event )
		{
			event                            = new cEvent< Targs... >();
			getInstance()->m_events[ _name ] = event;
		}

		event->subscribe( _object, _function );
	}

	template< typename T >
	void cEventManager::unsubscribe( const std::string& _name, T* _object )
	{
		ZoneScoped;

		const auto event = getInstance()->m_events[ _name ];

		if( event )
			event->unsubscribe( _object );
	}

	template< typename... Targs >
	void cEventManager::invoke( const std::string& _name, Targs... _args )
	{
		ZoneScoped;

		auto event = reinterpret_cast< cEvent< Targs... >* >( getInstance()->m_events[ _name ] );
		if( event )
			event->invoke( _args... );
	}
}

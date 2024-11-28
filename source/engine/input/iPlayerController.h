#pragma once

#include "engine/managers/cEventManager.h"
#include "Input.h"

namespace df
{
	class iPlayerController
	{
	public:
		DF_DISABLE_COPY_AND_MOVE( iPlayerController );

		iPlayerController()          = default;
		virtual ~iPlayerController() = default;

		void setActive( const bool _active )
		{
			ZoneScoped;

			m_active = _active;
			if( m_active )
				cEventManager::subscribe( event::input, this, &iPlayerController::input );
			else
				cEventManager::unsubscribe( event::input, this );
		}

		virtual void update( float /*_delta_time*/ ) {}
		virtual void input( const input::sInput& _input ) = 0;

	protected:
		bool m_active;
	};
}

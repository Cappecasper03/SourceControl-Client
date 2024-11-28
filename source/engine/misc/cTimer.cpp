#include "cTimer.h"

#include <tracy/Tracy.hpp>

namespace df
{
	cTimer::cTimer()
		: m_start( std::chrono::high_resolution_clock::now() )
		, m_last_update( std::chrono::high_resolution_clock::now() )
	{}

	double cTimer::getDeltaNano( const bool _update )
	{
		ZoneScoped;

		const std::chrono::time_point< std::chrono::steady_clock > now        = std::chrono::high_resolution_clock::now();
		const long long                                            delta_time = std::chrono::duration_cast< std::chrono::nanoseconds >( now - m_last_update ).count();

		if( _update )
			m_last_update = now;

		return static_cast< double >( delta_time );
	}

	double cTimer::getDeltaNano() const
	{
		ZoneScoped;

		const std::chrono::time_point< std::chrono::steady_clock > now        = std::chrono::high_resolution_clock::now();
		const long long                                            delta_time = std::chrono::duration_cast< std::chrono::nanoseconds >( now - m_last_update ).count();

		return static_cast< double >( delta_time );
	}

	double cTimer::getLifeNano() const
	{
		ZoneScoped;

		const std::chrono::time_point< std::chrono::steady_clock > now = std::chrono::high_resolution_clock::now();

		return static_cast< double >( std::chrono::duration_cast< std::chrono::nanoseconds >( now - m_start ).count() );
	}
}

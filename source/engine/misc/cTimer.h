#pragma once

#include <chrono>

namespace df
{
	class cTimer
	{
	public:
		cTimer();

		void update() { m_last_update = std::chrono::high_resolution_clock::now(); }

		double getDeltaSecond( const bool _update = true ) { return getDeltaNano( _update ) / 1'000'000'000; }
		double getDeltaMilli( const bool _update = true ) { return getDeltaNano( _update ) / 1'000'000; }
		double getDeltaMicro( const bool _update = true ) { return getDeltaNano( _update ) / 1'000; }
		double getDeltaNano( bool _update = true );

		double getDeltaSecond() const { return getDeltaNano() / 1'000'000'000; }
		double getDeltaMilli() const { return getDeltaNano() / 1'000'000; }
		double getDeltaMicro() const { return getDeltaNano() / 1'000; }
		double getDeltaNano() const;

		double getLifeSecond() const { return getLifeNano() / 1'000'000'000; }
		double getLifeMilli() const { return getLifeNano() / 1'000'000; }
		double getLifeMicro() const { return getLifeNano() / 1'000; }
		double getLifeNano() const;

	protected:
		std::chrono::high_resolution_clock::time_point m_start;
		std::chrono::high_resolution_clock::time_point m_last_update;
	};
}

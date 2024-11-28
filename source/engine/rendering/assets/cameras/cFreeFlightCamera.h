#pragma once

#include "cCamera.h"
#include "engine/input/iPlayerController.h"

namespace df
{
	class cFreeFlightCamera : public cCamera,
							  public iPlayerController
	{
	public:
		explicit cFreeFlightCamera( std::string _name, float _speed, float _sensitivity );

		void update( float _delta_time ) override;
		void input( const input::sInput& _input ) override;

	protected:
		float m_speed;
		float m_speed_multiplier;
		float m_sensitivity;

		glm::vec3 m_rotation;
		glm::vec3 m_position;
		glm::vec3 m_movement;
	};
}

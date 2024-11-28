#include "cFreeFlightCamera.h"

#include <GLFW/glfw3.h>
#include <glm/gtc/quaternion.hpp>

#include "engine/managers/cInputManager.h"

namespace df
{
	cFreeFlightCamera::cFreeFlightCamera( std::string _name, const float _speed, const float _sensitivity )
		: cCamera( std::move( _name ), ePerspective, cColor( .5f, .75f, 1, 1 ), 90 )
		, m_speed( _speed )
		, m_speed_multiplier( 1 )
		, m_sensitivity( _sensitivity )
		, m_rotation( .0f )
		, m_position( .0f )
		, m_movement( .0f )
	{}

	void cFreeFlightCamera::update( const float _delta_time )
	{
		ZoneScoped;

		if( m_movement.x != 0.f || m_movement.z != 0.f )
		{
			const glm::vec3 normalized_movement  = normalize( m_movement );
			m_position                          += glm::vec3( transform->world[ 0 ] ) * normalized_movement.x * m_speed * m_speed_multiplier * _delta_time;
			m_position                          += glm::vec3( transform->world[ 2 ] ) * normalized_movement.z * m_speed * m_speed_multiplier * _delta_time;
		}

		const glm::quat yaw_quaternion   = angleAxis( glm::radians( m_rotation.x ), glm::vec3( 1, 0, 0 ) );
		const glm::quat pitch_quaternion = angleAxis( glm::radians( m_rotation.y ), glm::vec3( 0, 1, 0 ) );
		const glm::mat4 rotation( mat4_cast( pitch_quaternion * yaw_quaternion ) );

		glm::mat4 translation( 1 );
		translation = translate( translation, m_position );

		transform->local = translation * rotation;

		cCamera::update();
	}

	void cFreeFlightCamera::input( const input::sInput& _input )
	{
		ZoneScoped;

		if( _input.mouse_cursor.updated )
		{
			m_rotation.x -= static_cast< float >( _input.mouse_cursor.y_delta ) * m_sensitivity;
			m_rotation.y -= static_cast< float >( _input.mouse_cursor.x_delta ) * m_sensitivity;
		}

		if( _input.mouse_scroll.updated )
		{
			if( _input.mouse_scroll.y_offset > 0 )
				m_speed_multiplier *= 2;
			else
				m_speed_multiplier *= .5f;

			if( m_speed_multiplier <= 0 )
				m_speed_multiplier = 1;
		}

		if( const input::eAction action = cInputManager::checkKey( GLFW_KEY_W ); action != input::eRepeat )
			m_movement.z -= static_cast< float >( action );

		if( const input::eAction action = cInputManager::checkKey( GLFW_KEY_S ); action != input::eRepeat )
			m_movement.z += static_cast< float >( action );

		if( const input::eAction action = cInputManager::checkKey( GLFW_KEY_A ); action != input::eRepeat )
			m_movement.x -= static_cast< float >( action );

		if( const input::eAction action = cInputManager::checkKey( GLFW_KEY_D ); action != input::eRepeat )
			m_movement.x += static_cast< float >( action );
	}
}

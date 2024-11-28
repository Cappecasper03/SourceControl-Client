#include "cShader_opengl.h"

#include <fmt/format.h>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <tracy/Tracy.hpp>

#include "engine/filesystem/cFileSystem.h"
#include "engine/log/Log.h"

namespace df::opengl
{
	cShader_opengl::cShader_opengl( std::string _name )
		: iShader( std::move( _name ) )
		, m_program( 0 )
	{
		ZoneScoped;

		const unsigned vertex   = compileShader( fmt::format( "{}.vert", name ), GL_VERTEX_SHADER );
		const unsigned fragment = compileShader( fmt::format( "{}.frag", name ), GL_FRAGMENT_SHADER );

		m_program = glCreateProgram();
		glAttachShader( m_program, vertex );
		glAttachShader( m_program, fragment );
		glLinkProgram( m_program );

		int success;
		glGetProgramiv( m_program, GL_LINK_STATUS, &success );

		if( success )
			DF_LOG_MESSAGE( fmt::format( "Successfully linked shader program: {}", name ) );
		else
		{
			char info_log[ 512 ];
			glGetProgramInfoLog( m_program, 512, nullptr, info_log );
			DF_LOG_ERROR( fmt::format( "Failed to link shader program: {} - {}", name, info_log ) );
		}

		glDeleteShader( vertex );
		glDeleteShader( fragment );
	}

	cShader_opengl::~cShader_opengl()
	{
		ZoneScoped;

		glDeleteProgram( m_program );
	}

	void cShader_opengl::use() const
	{
		ZoneScoped;

		glUseProgram( m_program );
	}

	void cShader_opengl::setUniform1B( const std::string& _name, const bool& _value ) const
	{
		ZoneScoped;

		glUniform1i( glGetUniformLocation( m_program, _name.data() ), _value );
	}

	void cShader_opengl::setUniform1I( const std::string& _name, const int& _value ) const
	{
		ZoneScoped;

		glUniform1i( glGetUniformLocation( m_program, _name.data() ), _value );
	}

	void cShader_opengl::setUniform1F( const std::string& _name, const float& _value ) const
	{
		ZoneScoped;

		glUniform1f( glGetUniformLocation( m_program, _name.data() ), _value );
	}

	void cShader_opengl::setUniform4F( const std::string& _name, const glm::vec4& _vector ) const
	{
		ZoneScoped;

		glUniform4f( glGetUniformLocation( m_program, _name.data() ), _vector.x, _vector.y, _vector.z, _vector.w );
	}

	void cShader_opengl::setUniform4F( const std::string& _name, const cColor& _color ) const
	{
		ZoneScoped;

		glUniform4f( glGetUniformLocation( m_program, _name.data() ), _color.r, _color.g, _color.b, _color.a );
	}

	void cShader_opengl::setUniformMatrix4F( const std::string& _name, const glm::mat4& _matrix, const int& _amount, const bool& _transpose ) const
	{
		ZoneScoped;

		glUniformMatrix4fv( glGetUniformLocation( m_program, _name.data() ), _amount, _transpose, value_ptr( _matrix ) );
	}

	unsigned cShader_opengl::compileShader( const std::string& _name, const int& _type )
	{
		ZoneScoped;

		const std::string shader_string = filesystem::readContent( "binaries/shaders/opengl/" + _name, "\n" );
		const char*       shader        = shader_string.data();

		const unsigned shader_id = glCreateShader( _type );
		glShaderSource( shader_id, 1, &shader, nullptr );
		glCompileShader( shader_id );

		int success;
		glGetShaderiv( shader_id, GL_COMPILE_STATUS, &success );

		if( success )
			DF_LOG_MESSAGE( fmt::format( "Successfully compiled shader: {}", _name ) );
		else
		{
			char info_log[ 512 ];
			glGetShaderInfoLog( shader_id, 512, nullptr, info_log );
			DF_LOG_ERROR( fmt::format( "Failed to compile shader: {} - {}", _name, info_log ) );
		}

		return shader_id;
	}
}

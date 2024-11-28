#pragma once

#include <glm/matrix.hpp>
#include <string>

#include "engine/misc/cColor.h"
#include "engine/misc/Misc.h"
#include "engine/rendering/iShader.h"

namespace df::opengl
{
	class cShader_opengl final : public iShader
	{
	public:
		DF_DISABLE_COPY_AND_MOVE( cShader_opengl );

		explicit cShader_opengl( std::string _name );
		~cShader_opengl() override;

		void use() const;

		void setUniform1B( const std::string& _name, const bool& _value ) const;
		void setUniform1I( const std::string& _name, const int& _value ) const;
		void setUniform1F( const std::string& _name, const float& _value ) const;

		void setUniform4F( const std::string& _name, const glm::vec4& _vector ) const;
		void setUniform4F( const std::string& _name, const cColor& _color ) const;

		void setUniformMatrix4F( const std::string& _name, const glm::mat4& _matrix, const int& _amount = 1, const bool& _transpose = false ) const;

		void setUniformSampler( const std::string& _name, const int& _sampler ) const { setUniform1I( _name, _sampler ); }

	private:
		static unsigned compileShader( const std::string& _name, const int& _type );

		unsigned m_program;
	};
}

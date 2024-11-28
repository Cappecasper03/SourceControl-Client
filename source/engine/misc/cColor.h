#pragma once

namespace df
{
	class cColor
	{
	public:
		cColor( float _r, float _g, float _b, float _a );

		cColor operator+( const cColor& _other ) const { return { r + _other.r, g + _other.g, b + _other.b, a + _other.a }; }
		cColor operator-( const cColor& _other ) const { return { r - _other.r, g - _other.g, b - _other.b, a - _other.a }; }
		cColor operator*( const cColor& _other ) const { return { r * _other.r, g * _other.g, b * _other.b, a * _other.a }; }
		cColor operator/( const cColor& _other ) const { return { r / _other.r, g / _other.g, b / _other.b, a / _other.a }; }
		cColor operator+( const float _scalar ) const { return { r + _scalar, g + _scalar, b + _scalar, a + _scalar }; }
		cColor operator-( const float _scalar ) const { return { r - _scalar, g - _scalar, b - _scalar, a - _scalar }; }
		cColor operator*( const float _scalar ) const { return { r * _scalar, g * _scalar, b * _scalar, a * _scalar }; }
		cColor operator/( const float _scalar ) const { return { r / _scalar, g / _scalar, b / _scalar, a / _scalar }; }

		cColor& operator+=( const cColor& _other ) { return *this = { r + _other.r, g + _other.g, b + _other.b, a + _other.a }; }
		cColor& operator-=( const cColor& _other ) { return *this = { r - _other.r, g - _other.g, b - _other.b, a - _other.a }; }
		cColor& operator*=( const cColor& _other ) { return *this = { r * _other.r, g * _other.g, b * _other.b, a * _other.a }; }
		cColor& operator/=( const cColor& _other ) { return *this = { r / _other.r, g / _other.g, b / _other.b, a / _other.a }; }
		cColor& operator+=( const float _scalar ) { return *this = { r + _scalar, g + _scalar, b + _scalar, a + _scalar }; }
		cColor& operator-=( const float _scalar ) { return *this = { r - _scalar, g - _scalar, b - _scalar, a - _scalar }; }
		cColor& operator*=( const float _scalar ) { return *this = { r * _scalar, g * _scalar, b * _scalar, a * _scalar }; }
		cColor& operator/=( const float _scalar ) { return *this = { r / _scalar, g / _scalar, b / _scalar, a / _scalar }; }

		float r;
		float g;
		float b;
		float a;
	};

	namespace color
	{
		inline const cColor red     = { 1.0f, 0.0f, 0.0f, 1.0f };
		inline const cColor green   = { 0.0f, 1.0f, 0.0f, 1.0f };
		inline const cColor blue    = { 0.0f, 0.0f, 1.0f, 1.0f };
		inline const cColor black   = { 0.0f, 0.0f, 0.0f, 1.0f };
		inline const cColor white   = { 1.0f, 1.0f, 1.0f, 1.0f };
		inline const cColor yellow  = { 1.0f, 1.0f, 0.0f, 1.0f };
		inline const cColor magenta = { 1.0f, 0.0f, 1.0f, 1.0f };
		inline const cColor cyan    = { 0.0f, 1.0f, 1.0f, 1.0f };
		inline const cColor orange  = { 1.0f, 0.6f, 0.0f, 1.0f };
		inline const cColor purple  = { 0.5f, 0.0f, 0.5f, 1.0f };
		inline const cColor pink    = { 1.0f, 0.8f, 0.8f, 1.0f };
		inline const cColor brown   = { 0.6f, 0.2f, 0.2f, 1.0f };
		inline const cColor teal    = { 0.0f, 0.5f, 0.5f, 1.0f };
		inline const cColor gray    = { 0.5f, 0.5f, 0.5f, 1.0f };
	}
}

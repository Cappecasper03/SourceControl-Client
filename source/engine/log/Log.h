#pragma once

#include <string>

namespace df::log
{
	enum eType
	{
		eRaw,
		eMessage,
		eWarning,
		eError,
	};

	extern void print( eType _type, const char* _function, unsigned _line, const std::string& _message );
	extern void printFile( eType _type, const char* _function, unsigned _line, const std::string& _message );
	extern void printConsole( eType _type, const char* _function, unsigned _line, const std::string& _message );
}

#define DF_LOG_RAW( ... )     df::log::print( df::log::eType::eRaw, __FUNCTION__, __LINE__, std::string( __VA_ARGS__ ) )
#define DF_LOG_MESSAGE( ... ) df::log::print( df::log::eType::eMessage, __FUNCTION__, __LINE__, std::string( __VA_ARGS__ ) )
#define DF_LOG_WARNING( ... ) df::log::print( df::log::eType::eWarning, __FUNCTION__, __LINE__, std::string( __VA_ARGS__ ) )
#define DF_LOG_ERROR( ... )   df::log::print( df::log::eType::eError, __FUNCTION__, __LINE__, std::string( __VA_ARGS__ ) )
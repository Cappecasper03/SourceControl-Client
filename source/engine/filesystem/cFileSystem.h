#pragma once

#include <fstream>
#include <string>
#include <vector>

namespace df::filesystem
{
	extern void               setGameDirectory( const std::string& _path );
	extern const std::string& getGameDirectory();

	std::string getPath( const std::string& _path, const std::vector< std::string >& _folders = {} );

	extern std::fstream open( const std::string& _path, std::ios::openmode _openmode = std::ios::in );
	extern bool         exists( const std::string& _path );
	extern std::string  readAll( const std::string& _path, const std::string& _line_separator = "" );
	extern std::string  readContent( const std::string& _path, const std::string& _line_separator = "" );
	extern void         write( const std::string& _path, const std::string& _message, std::ios::openmode _openmode = std::ios::out );
	extern int          remove( const std::string& _path );
}

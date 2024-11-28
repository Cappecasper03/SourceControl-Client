#include <ft2build.h>
#include FT_FREETYPE_H

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <tracy/Tracy.hpp>

#include "application/cApplication.h"

#ifdef TRACY_ENABLE
void* operator new( size_t _size )
{
	void* ptr = malloc( _size );
	TracyAlloc( ptr, _size );
	return ptr;
}

void operator delete( void* _ptr ) noexcept
{
	TracyFree( _ptr );
	free( _ptr );
}
#endif

int WinMain()
{
	cApplication::initialize();
	cApplication::run();
	cApplication::deinitialize();

	return 0;
}

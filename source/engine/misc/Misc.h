#pragma once

#define DF_DISABLE_COPY( T )           \
	T( const T& )            = delete; \
	T& operator=( const T& ) = delete;

#define DF_DISABLE_MOVE( T )      \
	T( T&& )            = delete; \
	T& operator=( T&& ) = delete;

#define DF_DISABLE_COPY_AND_MOVE( T ) \
	DF_DISABLE_COPY( T );             \
	DF_DISABLE_MOVE( T );

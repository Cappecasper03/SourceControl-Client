#include "cTransform.h"

#include <tracy/Tracy.hpp>

#include "engine/log/Log.h"

namespace df
{
	cTransform::cTransform()
		: local( 1 )
		, world( 1 )
		, parent( nullptr )
	{}

	cTransform::~cTransform()
	{
		ZoneScoped;

		if( parent )
			removeParent();

		while( !children.empty() )
			removeChild( *children.front() );
	}

	void cTransform::update()
	{
		ZoneScoped;

		world = parent ? local * parent->world : local;

		for( cTransform* child: children )
			child->update();
	}

	bool cTransform::addChild( cTransform& _child )
	{
		ZoneScoped;

		if( this == &_child )
		{
			DF_LOG_ERROR( "Child can't be itself" );
			return false;
		}

		if( _child.parent )
		{
			DF_LOG_ERROR( "Child already have a parent" );
			return false;
		}

		children.push_back( &_child );
		_child.parent = this;
		return true;
	}

	bool cTransform::removeChild( cTransform& _child )
	{
		ZoneScoped;

		if( std::erase( children, &_child ) )
		{
			_child.parent = nullptr;
			return true;
		}

		DF_LOG_WARNING( "Child doesn't exist" );
		return false;
	}

	bool cTransform::removeParent()
	{
		ZoneScoped;

		if( !parent )
		{
			DF_LOG_WARNING( "Parent doesn't exist" );
			return false;
		}

		parent->removeChild( *this );
		return true;
	}

	bool cTransform::setParent( cTransform& _parent )
	{
		ZoneScoped;

		if( this == &_parent )
		{
			DF_LOG_WARNING( "Parent can't be itself" );
			return false;
		}

		if( this->parent )
		{
			DF_LOG_WARNING( "Already have a parent" );
			return false;
		}

		parent = &_parent;
		_parent.children.push_back( this );
		return true;
	}
}

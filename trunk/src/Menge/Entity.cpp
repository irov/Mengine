#	include "Entity.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	Entity::Entity()
		: m_eventOnUpdate(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Entity::~Entity()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::_update( float _timing )
	{
		Node::_update( _timing );

		if( m_eventOnUpdate )
		{
			this->callEvent( EVENT_UPDATE, "(f)", _timing );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Entity::_activate()
	{
		bool result = Node::_activate();

		m_eventOnUpdate = this->registerSelfEvent( EVENT_UPDATE, ("onUpdate") );

		this->registerSelfEvent( EVENT_ACTIVATE, ("onActivate") );
		this->registerSelfEvent( EVENT_DEACTIVATE, ("onDeactivate") );
		this->registerSelfEvent( EVENT_COMPILE, ("onCompile") );
		this->registerSelfEvent( EVENT_RELEASE, ("onRelease") );


		this->callEvent( EVENT_ACTIVATE, "()" );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::_deactivate()
	{
		Node::_deactivate();

		this->callEvent( EVENT_DEACTIVATE, "()" );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Entity::_compile()
	{
		bool result = false;

		result = Node::_compile();

		if( result )
		{
			this->callEvent( EVENT_COMPILE, "()" );
		}

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::_release()
	{
		Node::_release();

		this->callEvent( EVENT_RELEASE, "()" );
	}
}

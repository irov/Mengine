#	include "Entity.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	Entity::Entity()
		: m_onUpdateEvent( false )
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
	
		if( m_onUpdateEvent )
		{
			this->callEvent( EVENT_UPDATE, "(f)", _timing );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Entity::_activate()
	{
		bool result = Node::_activate();

		if( m_onUpdateEvent == false )
		{
			m_onUpdateEvent = this->registerEvent( EVENT_UPDATE, ("onUpdate") );
		}

		this->callMethod( ("onActivate"), "()" );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::_deactivate()
	{
		Node::_deactivate();

		this->callMethod( ("onDeactivate"), "()" );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Entity::_compile()
	{
		bool result = false;

		result = Node::_compile();

		if( result )
		{
			this->callMethod( ("onCompile"), "()" );
		}

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::_release()
	{
		Node::_release();

		this->callMethod( ("onRelease"), "()" );
	}
}

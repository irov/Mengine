#	include "Entity.h"

#   include "Interface/NodeInterface.h"

#	include "Logger/Logger.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    Entity::Entity()
		: m_scriptEventable(nullptr)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Entity::~Entity()
    {
    }
	//////////////////////////////////////////////////////////////////////////
	void Entity::setPrototype( const ConstString & _prototype )
	{
		m_prototype = _prototype;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & Entity::getPrototype() const
	{
		return m_prototype;
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::setScriptEventable( Eventable * _eventable )
	{
		m_scriptEventable = _eventable;
	}
	//////////////////////////////////////////////////////////////////////////
	Eventable * Entity::getScriptEventable() const
	{
		return m_scriptEventable;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Entity::_activate()
	{
		EVENTABLE_CALL(m_serviceProvider, m_scriptEventable, EVENT_PREPARATION)("(O)", this->getEmbed());

		bool successful = Node::_activate();

		return successful;
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::_afterActivate()
	{
		Node::_afterActivate();

		EVENTABLE_CALL(m_serviceProvider, m_scriptEventable, EVENT_ACTIVATE)("(O)", this->getEmbed());
	}
    //////////////////////////////////////////////////////////////////////////
    void Entity::_deactivate()
    {
        Node::_deactivate();

        EVENTABLE_CALL(m_serviceProvider, m_scriptEventable, EVENT_PREPARATION_DEACTIVATE)("(O)", this->getEmbed());
    }
	//////////////////////////////////////////////////////////////////////////
	void Entity::_afterDeactivate()
	{
		Node::_afterDeactivate();

		EVENTABLE_CALL(m_serviceProvider, m_scriptEventable, EVENT_DEACTIVATE)("(O)", this->getEmbed());
	}
	//////////////////////////////////////////////////////////////////////////
	bool Entity::_compile()
	{
		EVENTABLE_CALL(m_serviceProvider, m_scriptEventable, EVENT_COMPILE)("(O)", this->getEmbed());
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::_release()
	{
		EVENTABLE_CALL(m_serviceProvider, m_scriptEventable, EVENT_RELEASE)("(O)", this->getEmbed());
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::onCreate()
	{
		EVENTABLE_CALL(m_serviceProvider, m_scriptEventable, EVENT_CREATE)("(O)", this->getEmbed());
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::destroy()
	{
        if( this->isShallowGrave() == true )
        {
            NODE_SERVICE(m_serviceProvider)
                ->addHomeless( this );

			this->release();

            return;
        }

        EVENTABLE_CALL(m_serviceProvider, m_scriptEventable, EVENT_DESTROY)("(O)", this->getEmbed());

        Factorable::destroy();
	}
}
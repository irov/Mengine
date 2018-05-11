#pragma once

#include "Interface/PrototypeManagerInterface.h"

#include "Kernel/Node.h"
#include "Kernel/Eventable.h"

#include "pybind/object.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum EntityEventFlag
    {
        EVENT_ENTITY_CREATE = 0,
        EVENT_ENTITY_DESTROY,
        EVENT_ENTITY_PREPARATION,
        EVENT_ENTITY_ACTIVATE,
        EVENT_ENTITY_PREPARATION_DEACTIVATE,
        EVENT_ENTITY_DEACTIVATE,
        EVENT_ENTITY_COMPILE,
        EVENT_ENTITY_RELEASE,
        __EVENT_ENTITY_LAST__
    };
    //////////////////////////////////////////////////////////////////////////
    class EntityEventReceiver
        : public EventReceiver
    {
    public:
        virtual void onEntityPreparation( const pybind::object & _self ) = 0;
        virtual void onEntityActivate( const pybind::object & _self ) = 0;
        virtual void onEntityPreparationDeactivate( const pybind::object & _self ) = 0;
        virtual void onEntityDeactivate( const pybind::object & _self ) = 0;
        virtual void onEntityCompile( const pybind::object & _self ) = 0;
        virtual void onEntityRelease( const pybind::object & _self ) = 0;
        virtual void onEntityCreate( const pybind::object & _self, Node * _node ) = 0;
        virtual void onEntityDestroy( const pybind::object & _self ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<EntityEventReceiver> EntityEventReceiverPtr;
    //////////////////////////////////////////////////////////////////////////
	class Entity
		: public Node
	{
    public:
        Entity();
        ~Entity() override;

	public:
		void setPrototype( const ConstString & _prototype );
		const ConstString & getPrototype() const;

	public:
		void setScriptEventable( Eventable * _eventable );
		Eventable * getScriptEventable() const;

	public:
		void setScriptObject( const pybind::object & _object );
		const pybind::object & getScriptObject() const;
		
    public:
        void onCreate();

	protected:
		bool _activate() override;
		void _afterActivate() override;
        
        void _deactivate() override;
		void _afterDeactivate() override;
		
        bool _compile() override;
		void _release() override;
			
	public:
		void destroy() override;
		void _destroy() override;

	protected:
		void _unshallowGrave() override;

	protected:
		ConstString m_prototype;

		Eventable * m_scriptEventable;
		pybind::object m_object;
	};
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Entity> EntityPtr;
    //////////////////////////////////////////////////////////////////////////
}
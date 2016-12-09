#	pragma once

#   include "Interface/PrototypeManagerInterface.h"

#	include "Node.h"

namespace Menge
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
        virtual void onEntityPreparation() = 0;
        virtual void onEntityActivate() = 0;
        virtual void onEntityPreparationDeactivate() = 0;
        virtual void onEntityDeactivate() = 0;
        virtual void onEntityCompile() = 0;
        virtual void onEntityRelease() = 0;
        virtual void onEntityCreate() = 0;
        virtual void onEntityDestroy() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<EntityEventReceiver> EntityEventReceiverPtr;
    //////////////////////////////////////////////////////////////////////////
	class Entity
		: public Node
		, public Eventable
	{
        EVENT_RECEIVER( EntityEventReceiver );

    public:
        Entity();
        ~Entity();

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
}
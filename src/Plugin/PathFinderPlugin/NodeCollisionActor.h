#pragma once

#include "Kernel/Node.h"

#include "CollisionWorld.h"
#include "CollisionActor.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class NodeCollisionActorUserData
        : public FactorablePtr
    {
    };
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<NodeCollisionActorUserData> NodeCollisionActorUserDataPtr;
    //////////////////////////////////////////////////////////////////////////
    enum CollisionEventFlag
    {
        EVENT_COLLISION_TEST = 0
    };
    //////////////////////////////////////////////////////////////////////////
    class NodeCollisionActorEventReceiver
        : public EventReceiver
    {
    public:
        virtual bool onNodeCollisionActorCollisionTest( class NodeCollisionActor * _other, uint32_t _iff1, uint32_t _iff2, const mt::vec3f & _point, const mt::vec3f & _normal, float _penetration ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
	class NodeCollisionActor
		: public Node
		, public Eventable
		, public CollisionActorProviderInterface
	{
        EVENT_RECEIVER( NodeCollisionActorEventReceiver );

	public:
		NodeCollisionActor();
		~NodeCollisionActor();

	public:
		void setCollisionRadius( float _collisionRadius );
		float getCollisionRadius() const;

	public:
		void setCollisionRaycast( const mt::vec3f & _raycastDirection );
		const mt::vec3f & getCollisionRaycast() const;

	public:
		void setCollisionIFF( uint32_t _collisionIFF );
		uint32_t getCollisionIFF() const;

	public:
		void setCollisionActive( bool _collisionActive );
		bool getCollisionActive() const;

	public:
		void addCollisionException( NodeCollisionActor * _actor );

	public:
		void setCollisionWorld( const CollisionWorldPtr & _collision );
		const CollisionWorldPtr & getCollisionWorld() const;

	public:
		void setCollisionUserData( const NodeCollisionActorUserDataPtr & _data );
		const NodeCollisionActorUserDataPtr & getCollisionUserData() const;

	protected:
		void onCollisionPositionProvider( mt::vec3f & _position ) const override;
		bool onCollisionTest( CollisionActorProviderInterface * _actor, const mt::vec3f & _point, const mt::vec3f & _normal, float _penetration ) override;

	protected:
		bool _activate() override;
		void _deactivate() override;

	protected:
		//void _setEventListener( const pybind::dict & _listener ) override;

	protected:
		void _debugRender(  RenderServiceInterface * _renderService, const RenderObjectState * _state, uint32_t _debugMask ) override;

	protected:
		CollisionWorldPtr m_collisionWorld;

		CollisionActorPtr m_actor;

		typedef stdex::vector<CollisionActorPtr> TVectorActorException;
		TVectorActorException m_exceptions;

        NodeCollisionActorUserDataPtr m_data;

		float m_collisionRadius;
		mt::vec3f m_collisionRaycastDirection;
		uint32_t m_collisionIFF;
		bool m_collisionActive;
	};
}
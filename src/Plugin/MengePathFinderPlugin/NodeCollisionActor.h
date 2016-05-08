#	pragma once

#	include "Kernel/Node.h"

#	include "CollisionInterface.h"

#	include "CollisionWorld.h"
#	include "CollisionActor.h"

namespace Menge
{
	class NodeCollisionActor
		: public Node
		, public CollisionActorProviderInterface
	{
	public:
		NodeCollisionActor();
		~NodeCollisionActor();

	public:
		void setCollisionRadius( float _collisionRadius );
		float getCollisionRadius() const;

	public:
		void setCollisionIFF( uint32_t _collisionIFF );
		uint32_t getCollisionIFF() const;

	public:
		void setCollisionActive( bool _collisionActive );
		bool getCollisionActive() const;

	public:
		void setCollisionWorld( const CollisionWorldPtr & _collision );
		const CollisionWorldPtr & getCollisionWorld() const;

	public:
		void setCollisionUserData( const pybind::object & _data );
		const pybind::object & getCollisionUserData() const;

	protected:
		void onCollisionPositionProvider( mt::vec2f & _position ) const override;
		bool onCollisionTest( CollisionActorProviderInterface * _actor, const mt::vec2f & _point, const mt::vec2f & _normal, float _penetration ) override;

	protected:
		bool _activate() override;
		void _deactivate() override;

	protected:
		void _setEventListener( const pybind::dict & _listener ) override;

	protected:
		void _debugRender( const RenderObjectState * _state, unsigned int _debugMask ) override;

	protected:
		CollisionWorldPtr m_collisionWorld;

		CollisionActorPtr m_actor;

		pybind::object m_data;

		float m_collisionRadius;
		uint32_t m_collisionIFF;
		bool m_collisionActive;
	};
}
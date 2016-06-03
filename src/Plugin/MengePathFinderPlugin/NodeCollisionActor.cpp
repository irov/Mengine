#	include "NodeCollisionActor.h"

#	include <math.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	NodeCollisionActor::NodeCollisionActor()
		: m_collisionRadius( 0.f )
		, m_collisionIFF( 0 )
		, m_collisionActive( false )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	NodeCollisionActor::~NodeCollisionActor()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeCollisionActor::setCollisionRadius( float _collisionRadius )
	{
		m_collisionRadius = _collisionRadius;

		if( m_actor != nullptr )
		{
			m_actor->setRadius( m_collisionRadius );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	float NodeCollisionActor::getCollisionRadius() const
	{
		return m_collisionRadius;
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeCollisionActor::setCollisionRaycast( const mt::vec2f & _raycastDirection )
	{
		m_raycastDirection = _raycastDirection;

		if( m_actor != nullptr )
		{
			m_actor->setRaycastDirection( m_raycastDirection );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & NodeCollisionActor::getCollisionRaycast() const
	{
		return m_raycastDirection;
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeCollisionActor::setCollisionIFF( uint32_t _collisionIFF )
	{
		m_collisionIFF = _collisionIFF;

		if( m_actor != nullptr )
		{
			m_actor->setIFF( m_collisionIFF );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t NodeCollisionActor::getCollisionIFF() const
	{
		return m_collisionIFF;
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeCollisionActor::setCollisionActive( bool _collisionActive )
	{
		m_collisionActive = _collisionActive;
	}
	//////////////////////////////////////////////////////////////////////////
	bool NodeCollisionActor::getCollisionActive() const
	{
		return m_collisionActive;
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeCollisionActor::addCollisionException( NodeCollisionActor * _actor )
	{
		if( _actor->isActivate() == false )
		{
			return;
		}

		const CollisionActorPtr & exception_actor = _actor->m_actor;

		m_exceptions.push_back( exception_actor );
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeCollisionActor::setCollisionWorld( const CollisionWorldPtr & _collision )
	{
		m_collisionWorld = _collision;
	}
	//////////////////////////////////////////////////////////////////////////
	const CollisionWorldPtr & NodeCollisionActor::getCollisionWorld() const
	{
		return m_collisionWorld;
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeCollisionActor::setCollisionUserData( const pybind::object & _data )
	{
		m_data = _data;
	}
	//////////////////////////////////////////////////////////////////////////
	const pybind::object & NodeCollisionActor::getCollisionUserData() const
	{
		return m_data;
	}
	//////////////////////////////////////////////////////////////////////////
	enum CollisionEventFlag
	{
		EVENT_COLLISION_TEST = 0
	};
	//////////////////////////////////////////////////////////////////////////
	void NodeCollisionActor::_setEventListener( const pybind::dict & _listener )
	{
		Node::_setEventListener( _listener );

		this->registerEvent( EVENT_COLLISION_TEST, ("onCollisionTest"), _listener );
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeCollisionActor::onCollisionPositionProvider( mt::vec2f & _position ) const
	{
		const mt::vec3f & wp = this->getWorldPosition();

		_position.x = wp.x;
		_position.y = wp.y;
	}
	//////////////////////////////////////////////////////////////////////////
	bool NodeCollisionActor::onCollisionTest( CollisionActorProviderInterface * _actor, const mt::vec2f & _point, const mt::vec2f & _normal, float _penetration )
	{
		NodeCollisionActor * actor = static_cast<NodeCollisionActor *>(_actor);

		uint32_t actor_iff = actor->getCollisionIFF();

		bool handle = true;

		EVENTABLE_ASK( m_serviceProvider, this, EVENT_COLLISION_TEST, handle )(this, actor, m_collisionIFF, actor_iff, _point, _normal, _penetration );

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool NodeCollisionActor::_activate()
	{
		if( m_collisionWorld == nullptr )
		{
			return false;
		}

		const CollisionActorPtr & actor = m_collisionWorld->createActor( this, m_collisionRadius, m_raycastDirection, m_collisionIFF, m_collisionActive );

		for( TVectorActorException::iterator
			it = m_exceptions.begin(),
			it_end = m_exceptions.end();
		it != it_end;
		++it )
		{
			const CollisionActorPtr & exception = *it;

			actor->addException( exception );
		}

		m_exceptions.clear();

		m_actor = actor;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeCollisionActor::_deactivate()
	{
		m_collisionWorld->removeActor( m_actor );
		m_actor = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeCollisionActor::_debugRender( const RenderObjectState * _state, unsigned int _debugMask )
	{
		(void)_debugMask;

		mt::vec2f v[16 + 1];

		for( uint32_t i = 0; i != 16; ++i )
		{
			v[i].x = m_collisionRadius * ::cosf( mt::m_two_pi / 16.f * i );
			v[i].y = m_collisionRadius * ::sinf( mt::m_two_pi / 16.f * i );
		}

		v[16] = v[0];

		RenderVertex2D * vertices = RENDER_SERVICE( m_serviceProvider )
			->getDebugRenderVertex2D( 17 * 2 );

		const mt::mat4f & wm = this->getWorldMatrix();

		for( uint32_t i = 0; i != 16; ++i )
		{
			size_t j = (i + 1) % 16;

			RenderVertex2D & v0 = vertices[i * 2 + 0];
			RenderVertex2D & v1 = vertices[i * 2 + 1];

			mt::mul_v3_v2_m4( v0.position, v[i], wm );
			mt::mul_v3_v2_m4( v1.position, v[j], wm );

			v0.color = 0x8080FFFF;
			v1.color = 0x8080FFFF;
			
			for( uint32_t uv_index = 0; uv_index != MENGINE_RENDER_VERTEX_UV_COUNT; ++uv_index )
			{
				v0.uv[uv_index].x = 0.f;
				v0.uv[uv_index].y = 0.f;

				v1.uv[uv_index].x = 0.f;
				v1.uv[uv_index].y = 0.f;
			}
		}

		const RenderMaterialInterfacePtr & debugMaterial = RENDERMATERIAL_SERVICE( m_serviceProvider )
			->getDebugMaterial();

		RENDER_SERVICE( m_serviceProvider )->addRenderLine( _state, debugMaterial
			, vertices
			, 17 * 2
			, nullptr
			, true
			);
	}
}
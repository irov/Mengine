#	include "CollisionActor.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	CollisionActor::CollisionActor()
		: m_radius( 0.f )
		, m_iff(0)
		, m_currentPosition(0.f, 0.f)
		, m_prevPosition(0.f, 0.f)
		, m_active(false)
		, m_remove(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionActor::setCollisionActorProvider( CollisionActorProviderInterface * _provider )
	{
		m_provider = _provider;
	}
	//////////////////////////////////////////////////////////////////////////
	CollisionActorProviderInterface * CollisionActor::getCollisionActorProvider() const
	{
		return m_provider;
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionActor::setRadius( float _radius )
	{
		m_radius = _radius;
	}
	//////////////////////////////////////////////////////////////////////////
	float CollisionActor::getRadius() const
	{
		return m_radius;
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionActor::setIFF( uint32_t _iff )
	{
		m_iff = _iff;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t CollisionActor::getIFF() const
	{
		return m_iff;
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionActor::setActiove( bool _active )
	{
		m_active = _active;
	}
	bool CollisionActor::isActive() const
	{
		return m_active;
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionActor::initialize( const mt::vec2f & _position )
	{
		m_currentPosition = _position;
		m_prevPosition = _position;
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionActor::update( const mt::vec2f & _position )
	{
		m_prevPosition = m_currentPosition;
		m_currentPosition = _position;
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionActor::makeCapsule( mt::capsule2 & _capsule )
	{
		_capsule.segment.a = m_prevPosition;
		_capsule.segment.b = m_currentPosition;
		_capsule.radius = m_radius;
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionActor::remove()
	{
		m_remove = true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool CollisionActor::isRemoved() const
	{
		return m_remove;
	}
}
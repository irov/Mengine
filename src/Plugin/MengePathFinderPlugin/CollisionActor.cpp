#	include "CollisionActor.h"

#	include "Math/utils.h"

#	include <algorithm>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	CollisionActor::CollisionActor()
		: m_radius( 0.f )
		, m_iff(0)
		, m_raycastDirection( 0.f, 0.f, 0.f )
		, m_currentPosition( 0.f, 0.f, 0.f )
		, m_prevPosition( 0.f, 0.f, 0.f )
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
	void CollisionActor::setRaycastDirection( const mt::vec3f & _raycastDirection )
	{
		m_raycastDirection = _raycastDirection;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & CollisionActor::getRaycastDirection() const
	{
		return m_raycastDirection;
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
	void CollisionActor::addException( const CollisionActorPtr & _actor )
	{
		m_exceptions.push_back( _actor );
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionActor::removeException( const CollisionActorPtr & _actor )
	{
		TVectorActorException::iterator it_found = std::find( m_exceptions.begin(), m_exceptions.end(), _actor );
		
		if( it_found == m_exceptions.end() )
		{
			return;
		}

		m_exceptions.erase( it_found );
	}
	//////////////////////////////////////////////////////////////////////////
	bool CollisionActor::isException( const CollisionActorPtr & _actor ) const
	{
		if( m_exceptions.empty() == true )
		{
			return false;
		}

		TVectorActorException::const_iterator it_found = std::find( m_exceptions.begin(), m_exceptions.end(), _actor );

		if( it_found == m_exceptions.end() )
		{
			return false;
		}

		return true;
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
	void CollisionActor::initialize()
	{
		mt::vec3f position;
		m_provider->onCollisionPositionProvider( position );

		m_currentPosition = position;
		m_prevPosition = position;
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionActor::update()
	{
		mt::vec3f position;
		m_provider->onCollisionPositionProvider( position );

		float dot_raycastDirection = mt::dot_v3( m_raycastDirection );

		if( mt::equal_f_z( dot_raycastDirection ) == false )
		{
			m_prevPosition = position;
			m_currentPosition = position + m_raycastDirection;
		}
		else
		{
			m_prevPosition = m_currentPosition;
			m_currentPosition = position;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionActor::getSphereCCD( mt::vec3f & _position, float & _radius, mt::vec3f & _velocity ) const
	{
		_position = m_prevPosition;
		_radius = m_radius;
		_velocity = m_currentPosition - m_prevPosition;
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionActor::remove()
	{
		m_exceptions.clear();

		m_remove = true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool CollisionActor::isRemoved() const
	{
		return m_remove;
	}
}
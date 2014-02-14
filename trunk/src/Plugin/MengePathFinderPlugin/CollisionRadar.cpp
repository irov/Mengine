#	include "CollisionRadar.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	CollisionRadar::CollisionRadar()
		: m_radius(0.f)
		, m_exceptObject(nullptr)
		, m_dead(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	CollisionRadar::~CollisionRadar()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionRadar::setRadius( float _radius )
	{
		m_radius = _radius;
	}
	//////////////////////////////////////////////////////////////////////////
	float CollisionRadar::getRadius() const
	{
		return m_radius;
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionRadar::setExceptCollisionObject( CollisionObject * _object )
	{
		m_exceptObject = _object;
	}
	//////////////////////////////////////////////////////////////////////////
	CollisionObject * CollisionRadar::getExceptCollisionObject() const
	{
		return m_exceptObject;
	}
	//////////////////////////////////////////////////////////////////////////
	bool CollisionRadar::inside( CollisionObject * _object ) const
	{
		if( m_exceptObject == _object )
		{
			return false;
		}

		for( TVectorCollisionObjects::const_iterator
			it = m_objects.begin(),
			it_end = m_objects.end();
		it != it_end;
		++it )
		{
			const CollisionObjectDesc & desc = *it;

			if( desc.object == _object )
			{
				return true;
			}
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionRadar::insertObject( CollisionObject * _object )
	{
		if( m_exceptObject == _object )
		{
			return;
		}

		for( TVectorCollisionObjects::iterator
			it = m_objects.begin(),
			it_end = m_objects.end();
		it != it_end;
		++it )
		{
			CollisionObjectDesc & desc = *it;

			if( desc.object == _object )
			{
				return;
			}
		}

		bool result = this->onRadarObjectFilter( _object );

		CollisionObjectDesc desc;
		desc.real = result;
		desc.object = _object;

		m_objects.push_back( desc );

		this->onRadarObjectEnter( _object );
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionRadar::removeObject( CollisionObject * _object )
	{
		if( m_exceptObject == _object )
		{
			return;
		}

		for( TVectorCollisionObjects::iterator
			it = m_objects.begin(),
			it_end = m_objects.end();
		it != it_end;
		++it )
		{
			CollisionObjectDesc & desc = *it;

			if( desc.object == _object )
			{
				if( desc.real == true )
				{
					this->onRadarObjectLeave( _object );
				}

				m_objects.erase( it );
				break;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionRadar::setDead( bool _dead )
	{
		m_dead = _dead;
	}
	//////////////////////////////////////////////////////////////////////////
	bool CollisionRadar::getDead() const
	{
		return m_dead;
	}
	//////////////////////////////////////////////////////////////////////////
	bool CollisionRadar::onRadarObjectFilter( CollisionObject * _object )
	{
		//Empty

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionRadar::onRadarObjectEnter( CollisionObject * _object )
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionRadar::onRadarObjectLeave( CollisionObject * _object )
	{
		//Empty
	}
}

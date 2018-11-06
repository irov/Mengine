#include "MagnetActor.h"

#include "math/utils.h"

#include <algorithm>

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	MagnetActor::MagnetActor()
		: m_radius( 0.f )
		, m_force( 0.f )
		, m_iff( 0 )
		, m_velocity( 0.f, 0.f, 0.f )
		, m_affect( false)
		, m_active( false )
		, m_remove( false )		
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void MagnetActor::setMagnetActorProvider( MagnetActorProviderInterface * _provider )
	{
		m_provider = _provider;
	}
	//////////////////////////////////////////////////////////////////////////
	MagnetActorProviderInterface * MagnetActor::getMagnetActorProvider() const
	{
		return m_provider;
	}
	//////////////////////////////////////////////////////////////////////////
	void MagnetActor::setRadius( float _radius )
	{
		m_radius = _radius;
	}
	//////////////////////////////////////////////////////////////////////////
	float MagnetActor::getRadius() const
	{
		return m_radius;
	}
	//////////////////////////////////////////////////////////////////////////
	void MagnetActor::setForce( float _force )
	{
		m_force = _force;
	}
	//////////////////////////////////////////////////////////////////////////
	float MagnetActor::getForce() const
	{
		return m_force;
	}
	//////////////////////////////////////////////////////////////////////////
	void MagnetActor::setActive( bool _active )
	{
		m_active = _active;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MagnetActor::isActive() const
	{
		return m_active;
	}
	//////////////////////////////////////////////////////////////////////////
	void MagnetActor::setIFF( uint32_t _iff )
	{
		m_iff = _iff;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t MagnetActor::getIFF() const
	{
		return m_iff;
	}
	//////////////////////////////////////////////////////////////////////////
	void MagnetActor::getSphere( mt::vec3f & _position, float & _radius ) const
	{
		mt::vec3f position;
		m_provider->onMagnetPositionProvider( position );

		_position = position;
		_radius = m_radius;
	}
	//////////////////////////////////////////////////////////////////////////
	void MagnetActor::addException( const MagnetActorPtr & _actor )
	{
		m_exceptions.emplace_back( _actor );
	}
	//////////////////////////////////////////////////////////////////////////
	void MagnetActor::removeException( const MagnetActorPtr & _actor )
	{
		TVectorActorException::iterator it_found = std::find( m_exceptions.begin(), m_exceptions.end(), _actor );
		
		if( it_found == m_exceptions.end() )
		{
			return;
		}

		m_exceptions.erase( it_found );
	}
	//////////////////////////////////////////////////////////////////////////
	bool MagnetActor::isException( const MagnetActorPtr & _actor ) const
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
	void MagnetActor::addVelocity( const mt::vec3f & _velocity )
	{
		mt::vec3f current_dir;
		float current_speed = mt::norm_v3_v3_f( current_dir, m_velocity );

		mt::vec3f new_dir;
		float new_speed = mt::norm_v3_v3_f( new_dir, _velocity );

		m_velocity = new_dir * (current_speed + new_speed);
		
		m_affect = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MagnetActor::updateVelocity()
	{
		if( m_affect == false )
		{
			m_velocity = mt::vec3f( 0.f, 0.f, 0.f );						
		}
		else
		{
			m_provider->onMagnetAffect( m_velocity );
		}

		m_affect = false;
	}
	//////////////////////////////////////////////////////////////////////////
	void MagnetActor::remove()
	{
		m_exceptions.clear();

		m_remove = true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MagnetActor::isRemoved() const
	{
		return m_remove;
	}
}
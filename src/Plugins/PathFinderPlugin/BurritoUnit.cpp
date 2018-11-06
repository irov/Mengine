#include "BurritoUnit.h"

#include "math/ccd.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	BurritoUnit::BurritoUnit()
		: m_node( nullptr )
		, m_position( 0.f, 0.f, 0.f )
		, m_velocity( 0.f, 0.f, 0.f )
		, m_radius( 0.f )
		, m_dead( false )
		, m_collide( true )
		, m_bound( true )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	BurritoUnit::~BurritoUnit()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Node * BurritoUnit::getNode() const
	{
		return m_node;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & BurritoUnit::getPosition() const
	{
		return m_position;
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoUnit::setDead()
	{
		m_dead = true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool BurritoUnit::isDead() const
	{
		return m_dead;
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoUnit::setCollide( bool _value )
	{
		m_collide = _value;
	}
	//////////////////////////////////////////////////////////////////////////
	bool BurritoUnit::isCollide() const
	{
		return m_collide;
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoUnit::setBound( bool _value )
	{
		m_bound = _value;
	}
	//////////////////////////////////////////////////////////////////////////
	bool BurritoUnit::isBound() const
	{
		return m_bound;
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoUnit::initialize( Node * _node, const mt::vec3f & _position, const mt::vec3f & _velocity, float _radius, bool _collide, const pybind::object & _cb )
	{
		m_node = _node;
		m_position = _position;
		m_velocity = _velocity;
		m_radius = _radius;
		m_collide = _collide;

		m_cb = _cb;

		m_node->setLocalPosition( m_position );
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoUnit::setVelocity( const mt::vec3f & _velocity )
	{
		m_velocity = _velocity;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & BurritoUnit::getVelocity() const
	{
		return m_velocity;
	}
	//////////////////////////////////////////////////////////////////////////
	bool BurritoUnit::check_collision( float _timing, const mt::vec3f & _burritoPosition, float _burritoRadius, const mt::vec3f & _burritoVelocity, float & _collisionTiming ) const
	{ 
		if( m_dead == true )
		{
			return false;
		}

		if( m_collide == false )
		{
			return false;
		}

		float ccd_timing;
		mt::vec3f n;
		if( mt::ccd_sphere_sphere( _burritoPosition, _burritoRadius, _burritoVelocity, m_position, m_radius, m_velocity, ccd_timing, n ) == false )
		{
			return false;
		}

		if( ccd_timing > _timing )
		{
			return false;
		}

		_collisionTiming = _timing - ccd_timing;
		
		m_cb.call();

		return true;			 
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoUnit::translate( const mt::vec3f & _translate )
	{ 
		m_position += _translate;

		m_node->setLocalPosition( m_position );
	}
}
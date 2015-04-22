#	include "BurritoUnit.h"

#	include "Math/ccd.h"

#	include "pybind/pybind.hpp"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	BurritoUnit::BurritoUnit()
		: m_node(nullptr)
		, m_position(0.f, 0.f, 0.f)
		, m_velocity( 0.f, 0.f, 0.f )
		, m_radius(0.f)
		, m_cb(nullptr)
		, m_dead(false)
		, m_collide(true)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	BurritoUnit::~BurritoUnit()
	{
		pybind::decref( m_cb );
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
	bool BurritoUnit::isDead() const
	{
		return m_dead;
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoUnit::initialize( Node * _node, const mt::vec3f & _position, const mt::vec3f & _velocity, float _radius, bool _collide, PyObject * _cb )
	{
		m_node = _node;
		m_position = _position;
		m_velocity = _velocity;
		m_radius = _radius;
		m_collide = _collide;

		m_cb = _cb;
		pybind::incref( m_cb );

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
	bool BurritoUnit::check_collision( float _timing, const mt::vec3f & _burritoPosition, float _burritoRadius, const mt::vec3f & _burritoVelocity, float & _collisionTiming, mt::vec2f & _factor ) const
	{ 
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
		
		PyObject * py_result = pybind::ask_t(m_cb, m_node, _burritoVelocity);

		PyObject * py_factor = pybind::tuple_getitem( py_result, 0 );

		_factor = pybind::extract<mt::vec2f>( py_factor );

		uint32_t flag = pybind::tuple_getitem_t<uint32_t>( py_result, 1 );

		if( flag == 1 )
		{			
			m_dead = true;
		}
		else if( flag == 2 )
		{
			m_collide = false;
		}

		return true;			 
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoUnit::update( float _timing, const mt::vec3f & _translate )
	{
		if( m_dead == true )
		{
			return;
		}

		m_position += m_velocity * _timing + _translate;
		
		m_node->setLocalPosition( m_position );
	}	
}
#	include "BurritoBison.h"

#	include <algorithm>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		//////////////////////////////////////////////////////////////////////////
		struct FBurritoBisonImpulseDead
		{
			bool operator()( const BurritoBisonImpulse & _impulse ) const
			{
				return _impulse.time < 0.f;
			}
		};
		//////////////////////////////////////////////////////////////////////////
		struct FBurritoBisonForceRemove
		{
			FBurritoBisonForceRemove( const ConstString & _name )
				: name(_name)
			{
			}

			bool operator()( const BurritoBisonForce & _force ) const
			{
				return _force.name == name;
			}

			const ConstString & name;

		private:
			FBurritoBisonForceRemove & operator = ( const FBurritoBisonForceRemove & _name );
		};
	}	
	//////////////////////////////////////////////////////////////////////////
	BurritoBison::BurritoBison()
		: m_node(nullptr)
		, m_position( 0.f, 0.f, 0.f )
		, m_offset( 0.f, 0.f, 0.f )
		, m_bison_y( 0.f )
		, m_radius( 0.f )		
		, m_velocity( 0.f, 0.f, 0.f )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	BurritoBison::~BurritoBison()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoBison::initialize( Node * _node, const mt::vec3f & _offset, float _radius )
	{		
		m_node = _node;
		m_offset = _offset;
		m_radius = _radius;

		m_node->setLocalPosition( m_offset );
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoBison::addForce( const ConstString & _name, const mt::vec3f & _direction, float _value )
	{
		BurritoBisonForce force;
		force.name = _name;
		force.direction = _direction;
		force.value = _value;

		m_forces.push_back( force );
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoBison::removeForce( const ConstString & _name )
	{
		TVectorBurritoBisonForce::iterator it_erase = std::find_if( m_forces.begin(), m_forces.end(), FBurritoBisonForceRemove(_name) );
		m_forces.erase( it_erase );
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoBison::addImpulse( const mt::vec3f & _direction, float _value, float _time )
	{
		BurritoBisonImpulse impulse;
		impulse.direction = _direction;
		impulse.value = _value;
		impulse.time = _time;
		impulse.timing = _time;

		m_impulses.push_back( impulse );
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & BurritoBison::getOffset() const
	{
		return m_offset;
	}
	//////////////////////////////////////////////////////////////////////////
	float BurritoBison::getRadius() const
	{ 
		return m_radius;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & BurritoBison::getVelocity() const
	{
		return m_velocity;
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoBison::update( float _time, float _timing, mt::vec3f & _velocity, mt::vec3f & _position, mt::vec3f & _offset, mt::vec3f & _offsetH )
	{		
		(void)_time;

		mt::vec3f force_velocity(0.f, 0.f, 0.f);

		for( TVectorBurritoBisonForce::const_iterator
			it = m_forces.begin(),
			it_end = m_forces.end();
		it != it_end;
		++it )
		{
			const BurritoBisonForce & force = *it;

			force_velocity += force.direction * force.value * _timing;
		}

		for( TVectorBurritoBisonImpulse::const_iterator
			it = m_impulses.begin(),
			it_end = m_impulses.end();
		it != it_end;
		++it )
		{
			const BurritoBisonImpulse & impulse = *it;

			if( impulse.timing <= 0.f )
			{
				continue;
			}

			mt::vec3f force_impulse;

			if( impulse.timing - _timing > 0.f )
			{
				force_impulse = impulse.direction * impulse.value * impulse.timing / impulse.time * _timing;
			}
			else
			{
				force_impulse = impulse.direction * impulse.value * impulse.timing / impulse.time * impulse.timing;
			}

			force_velocity += force_impulse;

			impulse.timing -= _timing;
		}

		TVectorBurritoBisonImpulse::iterator it_erase = std::remove_if( m_impulses.begin(), m_impulses.end(), FBurritoBisonImpulseDead() );
		m_impulses.erase( it_erase, m_impulses.end() );

		m_velocity += force_velocity;

		_velocity = m_velocity;
		_position = m_position;		
		_offset = m_offset;
		_offsetH = m_offset + mt::vec3f( 0.f, m_bison_y, 0.f );
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoBison::translate( const mt::vec3f & _translate, mt::vec3f & _position )
	{ 
		m_position += _translate;

		if( m_position.y > 0.f )
		{
			m_bison_y = m_position.y;

			_position = mt::vec3f( _translate.x, 0.f, _translate.z );
		}
		else
		{
			m_bison_y = 0.f;

			_position = _translate;
		}

		m_node->setLocalPosition( m_offset + mt::vec3f( 0.f, m_bison_y, 0.f ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoBison::reflect( const mt::vec2f & _factor, mt::vec3f & _velocity )
	{
		m_velocity.x *= _factor.x;
		m_velocity.y *= _factor.y;

		_velocity = m_velocity;
	}
}
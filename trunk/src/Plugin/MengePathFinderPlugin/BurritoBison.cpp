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
		, m_radius(0.f)
		, m_velocity(0.f, 0.f, 0.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	BurritoBison::~BurritoBison()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoBison::initialize( Node * _node, float _radius )
	{
		m_node = _node;
		m_radius = _radius;
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
	const mt::vec3f & BurritoBison::getVelocity() const
	{
		return m_velocity;
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoBison::update_velocity( float _time, float _timing, mt::vec3f & _velocity )
	{		
		(void)_time;

		if( m_node == nullptr )
		{
			return;
		}

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

			if( impulse.timing - _timing > 0.f )
			{
				force_velocity += impulse.direction * impulse.value * impulse.time / impulse.timing * _timing;
			}
			else
			{
				force_velocity += impulse.direction * impulse.value * impulse.time;
			}

			impulse.timing -= _timing;
		}

		TVectorBurritoBisonImpulse::iterator it_erase = std::remove_if( m_impulses.begin(), m_impulses.end(), FBurritoBisonImpulseDead() );
		m_impulses.erase( it_erase, m_impulses.end() );

		m_velocity += force_velocity;

		//m_node->translate( m_velocity );

		_velocity = m_velocity;
	}
}
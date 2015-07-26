#	include "BurritoBison.h"

#	include "pybind/pybind.hpp"

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
		//////////////////////////////////////////////////////////////////////////
		struct FBurritoBisonVelocityEventDead
		{
			bool operator()( const VelocityEventDesc & _event ) const
			{
				return _event.dead == true;
			}
		};
		//////////////////////////////////////////////////////////////////////////
		struct FBurritoBisonDistanceEventDead
		{
			bool operator()( const DistanceEventDesc & _event ) const
			{
				return _event.dead == true;
			}
		};
	}	
	//////////////////////////////////////////////////////////////////////////
	BurritoBison::BurritoBison()
		: m_node(nullptr)
		, m_position( 0.f, 0.f, 0.f )
		, m_offset( 0.f, 0.f, 0.f )
		, m_bisonY( 0.f )
		, m_radius( 0.f )		
		, m_velocity( 0.f, 0.f, 0.f )
		, m_neutron( false )
		, m_collide( true )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	BurritoBison::~BurritoBison()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoBison::initialize( Node * _node, const mt::vec3f & _offset, float _bisonY, float _radius )
	{		
		m_node = _node;
		m_offset = _offset;
		m_radius = _radius;
		m_bisonY = _bisonY;

		m_node->setLocalPosition( m_offset + mt::vec3f( 0.f, m_bisonY, 0.f ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoBison::finalize()
	{

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
		
		impulse.direction;
		mt::norm_v3( impulse.direction, _direction );

		impulse.value = _value;
		impulse.time = _time;
		impulse.timing = _time;

		m_impulses.push_back( impulse );
	}
	//////////////////////////////////////////////////////////////////////////
	Node * BurritoBison::getNode() const
	{ 
		return m_node;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & BurritoBison::getPosition() const
	{
		return m_position;
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
	void BurritoBison::setVelocity( const mt::vec3f & _velocity )
	{ 
		m_velocity = _velocity;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & BurritoBison::getVelocity() const
	{
		return m_velocity;
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoBison::setNeutron( bool _value )
	{
		m_neutron = _value;
	}
	//////////////////////////////////////////////////////////////////////////
	bool BurritoBison::getNeutron() const
	{
		return m_neutron;
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoBison::setCollide( bool _value )
	{
		m_collide = _value;
	}
	//////////////////////////////////////////////////////////////////////////
	bool BurritoBison::getCollide() const
	{
		return m_collide;
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoBison::update( float _time, float _timing, mt::vec3f & _velocity, mt::vec3f & _offset, mt::vec3f & _offsetH, uint32_t _iterate )
	{		
		(void)_time;

		if( m_neutron == true )
		{
			_velocity = mt::vec3f( m_velocity.x, 0.f, m_velocity.z );
			_offset = m_offset;
			_offsetH = m_offset + mt::vec3f( 0.f, m_bisonY, 0.f );

			return;
		}

		if( _iterate == 0 )
		{
			mt::vec3f force_velocity( 0.f, 0.f, 0.f );

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

				force_impulse = impulse.direction * impulse.value * mt::ltrim_f( _timing, 0.f );

				impulse.timing -= _timing;

				force_velocity += force_impulse;
			}

			TVectorBurritoBisonImpulse::iterator it_erase = std::remove_if( m_impulses.begin(), m_impulses.end(), FBurritoBisonImpulseDead() );
			m_impulses.erase( it_erase, m_impulses.end() );

			m_velocity += force_velocity;

			m_velocityEvents.insert( m_velocityEvents.end(), m_velocityEventsAdd.begin(), m_velocityEventsAdd.end() );
			m_velocityEventsAdd.clear();

			for( TVectorVelocityEventDesc::iterator
				it = m_velocityEvents.begin(),
				it_end = m_velocityEvents.end();
			it != it_end;
			++it )
			{
				VelocityEventDesc & desc = *it;

				if( desc.dead == true )
				{
					continue;
				}

				bool test = desc.test;

				desc.test = this->testVelocityEvent_( desc );

				if( test == false && desc.test == true )
				{
					bool result = desc.cb();

					desc.dead = result;
				}
			}

			TVectorVelocityEventDesc::iterator it_event_erase = std::remove_if( m_velocityEvents.begin(), m_velocityEvents.end(), FBurritoBisonVelocityEventDead() );
			m_velocityEvents.erase( it_event_erase, m_velocityEvents.end() );

			m_distanceEvents.insert( m_distanceEvents.end(), m_distanceEventsAdd.begin(), m_distanceEventsAdd.end() );
			m_distanceEventsAdd.clear();

			for( TVectorDistanceEventDesc::iterator
				it = m_distanceEvents.begin(),
				it_end = m_distanceEvents.end();
			it != it_end;
			++it )
			{
				DistanceEventDesc & desc = *it;

				if( desc.dead == true )
				{
					continue;
				}

				bool repeat = false;

				do
				{
					repeat = false;

					desc.distance -= m_velocity.x * _timing;

					if( desc.distance > 0.f )
					{
						continue;
					}

					bool result = desc.cb();

					if( result == true )
					{
						repeat = true;

						desc.distance += desc.init_distance;
					}
					else
					{
						desc.dead = true;
					}
				}
				while( repeat == true );
			}

			TVectorDistanceEventDesc::iterator it_distance_event_erase = std::remove_if( m_distanceEvents.begin(), m_distanceEvents.end(), FBurritoBisonDistanceEventDead() );
			m_distanceEvents.erase( it_distance_event_erase, m_distanceEvents.end() );
		}

		_velocity = m_velocity;
	
		_offset = m_offset;
		_offsetH = m_offset + mt::vec3f( 0.f, m_bisonY, 0.f );
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoBison::translate( const mt::vec3f & _translate, mt::vec3f & _position )
	{ 
		if( m_position.y <= 0.f && m_position.y + _translate.y <= 0.f )
		{
			m_bisonY = 0.f;

			_position = -_translate;
		}
		else if( m_position.y <= 0.f && m_position.y + _translate.y >= 0.f )
		{
			m_bisonY = m_position.y + _translate.y;

			_position = -mt::vec3f( _translate.x, -m_position.y, _translate.z );
		}
		else if( m_position.y >= 0.f && m_position.y + _translate.y >= 0.f )
		{
			m_bisonY = m_position.y;

			_position = -mt::vec3f( _translate.x, 0.f, _translate.z );
		}
		else if( m_position.y >= 0.f && m_position.y + _translate.y <= 0.f )
		{
			m_bisonY = 0.f;

			_position = -mt::vec3f( _translate.x, m_position.y + _translate.y, _translate.z );
		}
		else
		{
			printf( "AAAAAAAAAAAAHHHTUNG!!!!" );
		}

		m_node->setLocalPosition( m_offset + mt::vec3f( 0.f, m_bisonY, 0.f ) );		

		m_position += _translate;
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoBison::addVelocityEvent( bool _less, const mt::vec3f & _velocity, const pybind::object & _cb )
	{ 
		VelocityEventDesc desc;
		desc.less = _less;
		desc.velocity_sqrlength = mt::norm_v3_f( desc.velocity, _velocity );
		
		desc.cb = _cb;
		
		desc.test = this->testVelocityEvent_( desc );
		desc.dead = false;
			
		m_velocityEventsAdd.push_back( desc );
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoBison::removeAllVelocityEvents()
	{ 
		for( TVectorVelocityEventDesc::iterator
			it = m_velocityEvents.begin(),
			it_end = m_velocityEvents.end();
		it != it_end;
		++it )
		{
			VelocityEventDesc & desc = *it;

			desc.dead = true;
		}

		m_velocityEventsAdd.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoBison::addDistanceEvent( float _distance, const pybind::object & _cb )
	{
		DistanceEventDesc desc;
		desc.init_distance = _distance;
		desc.distance = _distance;
		desc.cb = _cb;
		desc.dead = false;

		m_distanceEventsAdd.push_back( desc );
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoBison::removeAllDistanceEvents()
	{
		for( TVectorDistanceEventDesc::iterator
			it = m_distanceEvents.begin(),
			it_end = m_distanceEvents.end();
		it != it_end;
		++it )
		{
			DistanceEventDesc & desc = *it;

			desc.dead = true;
		}

		m_distanceEventsAdd.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	bool BurritoBison::testVelocityEvent_( const VelocityEventDesc & _desc ) const
	{
		float d = mt::dot_v3_v3( _desc.velocity, m_velocity );
		float l = _desc.velocity_sqrlength;

		return _desc.less ? d <= l : d >= l;		
	}
}
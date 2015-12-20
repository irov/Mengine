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
		struct FBurritoBisonHeightEventDead
		{
			bool operator()( const HeightEventDesc & _event ) const
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
		, m_radius( 0.f )		
		, m_velocity( 0.f, 0.f, 0.f )
		, m_neutron( false )
		, m_collide( true )
		, m_enumeratorId(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	BurritoBison::~BurritoBison()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoBison::initialize( Node * _node, Camera2D * _camera, const Viewport & _renderport, const mt::vec3f & _position, float _radius )
	{		
		m_node = _node;
		m_camera = _camera;
		m_renderport = _renderport;
		m_position = _position;
		m_radius = _radius;

		m_node->setLocalPosition( _position );
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoBison::finalize()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool BurritoBison::addForce( const ConstString & _name, const mt::vec3f & _direction, float _value )
	{
		TVectorBurritoBisonForce::iterator it_erase = std::find_if( m_forces.begin(), m_forces.end(), FBurritoBisonForceRemove( _name ) );

		if( it_erase != m_forces.end() )
		{
			return false;
		}

		BurritoBisonForce force;
		force.name = _name;
		force.direction = _direction;
		force.value = _value;

		m_forces.push_back( force );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool BurritoBison::removeForce( const ConstString & _name )
	{
		TVectorBurritoBisonForce::iterator it_erase = std::find_if( m_forces.begin(), m_forces.end(), FBurritoBisonForceRemove(_name) );

		if( it_erase == m_forces.end() )
		{
			return false;
		}

		m_forces.erase( it_erase );

		return true;
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
	void BurritoBison::update( float _time, float _timing, mt::vec3f & _velocity, mt::vec3f & _position, uint32_t _iterate )
	{		
		(void)_time;

		if( m_neutron == true )
		{
			_velocity = mt::vec3f( m_velocity.x, 0.f, m_velocity.z );

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

			m_heightEvents.insert( m_heightEvents.end(), m_heightEventsAdd.begin(), m_heightEventsAdd.end() );
			m_heightEventsAdd.clear();

			m_heightEvents.erase( 
				std::remove_if( m_heightEvents.begin(), m_heightEvents.end(), FBurritoBisonHeightEventDead() )
				, m_heightEvents.end() 
				);

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

			m_velocityEvents.erase( 
				std::remove_if( m_velocityEvents.begin(), m_velocityEvents.end(), FBurritoBisonVelocityEventDead() )
				, m_velocityEvents.end() 
				);

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

				desc.distance -= m_velocity.x * _timing;

				bool repeat = false;

				do
				{
					if( desc.dead == true )
					{
						break;
					}					

					if( desc.distance > 0.f )
					{
						break;
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
			
			m_distanceEvents.erase( 
				std::remove_if( m_distanceEvents.begin(), m_distanceEvents.end(), FBurritoBisonDistanceEventDead() )
				, m_distanceEvents.end() 
				);
		}

		_velocity = m_velocity;

		const mt::vec3f & position = m_node->getLocalPosition();

		_position = position;
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoBison::translate( const mt::vec3f & _translate )
	{ 
		m_position += _translate;

		for( TVectorHeightEventDesc::iterator
			it = m_heightEvents.begin(),
			it_end = m_heightEvents.end();
		it != it_end;
		++it )
		{
			HeightEventDesc & desc = *it;

			if( desc.dead == true )
			{
				continue;
			}

			bool test = desc.test;

			desc.test = this->testHeightEvent_( desc );

			if( test == false && desc.test == true )
			{
				bool result = desc.cb();

				desc.dead = result;
			}
		}

		m_node->setLocalPositionY( m_position.y );

		float a = (m_velocity.x - 3.f) / (6.f - 3.f);

		float ca = 1.f + mt::clamp( 0.f, a, 1.f ) * 0.5f;

		Viewport new_camera_renderport;
		new_camera_renderport = m_renderport;
		new_camera_renderport.inflate( mt::vec2f( 1.f, ca ) );
		new_camera_renderport.scale( mt::vec2f( ca, 1.f ) );

		m_camera->setRenderport( new_camera_renderport );
		
		const mt::vec3f & target_wp = m_node->getWorldPosition();

		const mt::vec3f & camera_lp = m_camera->getLocalPosition();
		const Viewport & camera_renderport = m_camera->getCameraRenderport();

		Viewport camera_renderport_wm;
		camera_renderport_wm.begin = camera_renderport.begin + target_wp.to_vec2f();
		camera_renderport_wm.end = camera_renderport.end + target_wp.to_vec2f();

		mt::vec2f camera_renderport_wm_center;
		camera_renderport_wm.getCenter( camera_renderport_wm_center );

		float camera_height = camera_renderport.getHeight();
		float camera_height_half = camera_height * 0.5f;

		float renderport_height = m_renderport.getHeight();
		float camera_renderport_height = camera_renderport.getHeight();

		float pr = (camera_renderport_height - (target_wp.y - 300.f) * 1.f) / camera_renderport_height;

		mt::vec3f new_camera_lp;

		if( pr < 1.f )
		{
			new_camera_lp.x = 0.f;
			new_camera_lp.y = (renderport_height - camera_renderport_height) * 0.5f;
			new_camera_lp.z = 0.f;
		}
		else
		{
			new_camera_lp.x = 0.f;
			new_camera_lp.y = (renderport_height - camera_renderport_height) * 0.5f + target_wp.y - 300.f;
			new_camera_lp.z = 0.f;
		}

		m_camera->setLocalPosition( new_camera_lp );
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoBison::setPositionY( float _position )
	{
		m_position.y = _position;

		m_node->setLocalPositionY( m_position.y );
	}
	//////////////////////////////////////////////////////////////////////////
	float BurritoBison::getPositionY() const
	{
		return m_position.y;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t BurritoBison::addVelocityEvent( bool _less, const mt::vec3f & _velocity, const pybind::object & _cb )
	{ 
		VelocityEventDesc desc;
		desc.id = ++m_enumeratorId;		
		desc.velocity_sqrlength = mt::norm_v3_f( desc.velocity, _velocity );
		
		desc.cb = _cb;

		desc.less = _less;
		desc.test = this->testVelocityEvent_( desc );
		desc.dead = false;
			
		m_velocityEventsAdd.push_back( desc );

		return desc.id;
	}
	//////////////////////////////////////////////////////////////////////////
	bool BurritoBison::removeVelocityEvent( uint32_t _eventId )
	{
		for( TVectorVelocityEventDesc::iterator
			it = m_velocityEvents.begin(),
			it_end = m_velocityEvents.end();
		it != it_end;
		++it )
		{
			VelocityEventDesc & desc = *it;

			if( desc.id != _eventId )
			{
				continue;
			}

			desc.dead = true;

			return true;
		}

		return false;
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
	uint32_t BurritoBison::addHeightEvent( bool _less, float _height, const pybind::object & _cb )
	{
		HeightEventDesc desc;
		desc.id = ++m_enumeratorId;
		desc.height = _height;

		desc.cb = _cb;

		desc.less = _less;
		desc.test = this->testHeightEvent_( desc );
		desc.dead = false;

		m_heightEventsAdd.push_back( desc );

		return desc.id;
	}
	//////////////////////////////////////////////////////////////////////////
	bool BurritoBison::removeHeightEvent( uint32_t _eventId )
	{
		for( TVectorHeightEventDesc::iterator
			it = m_heightEvents.begin(),
			it_end = m_heightEvents.end();
		it != it_end;
		++it )
		{
			HeightEventDesc & desc = *it;

			if( desc.id != _eventId )
			{
				continue;
			}

			desc.dead = true;

			return true;
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoBison::removeAllHeightEvents()
	{
		for( TVectorHeightEventDesc::iterator
			it = m_heightEvents.begin(),
			it_end = m_heightEvents.end();
		it != it_end;
		++it )
		{
			HeightEventDesc & desc = *it;

			desc.dead = true;
		}

		m_heightEventsAdd.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t BurritoBison::addDistanceEvent( float _distance, const pybind::object & _cb )
	{
		DistanceEventDesc desc;
		desc.id = ++m_enumeratorId;
		desc.init_distance = _distance;
		desc.distance = _distance;
		desc.cb = _cb;
		desc.dead = false;

		m_distanceEventsAdd.push_back( desc );

		return desc.id;
	}
	//////////////////////////////////////////////////////////////////////////
	bool BurritoBison::removeDistanceEvent( uint32_t _eventId )
	{
		for( TVectorDistanceEventDesc::iterator
			it = m_distanceEvents.begin(),
			it_end = m_distanceEvents.end();
		it != it_end;
		++it )
		{
			DistanceEventDesc & desc = *it;

			if( desc.id != _eventId )
			{
				continue;
			}

			desc.dead = true;

			return true;
		}

		return false;
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
	//////////////////////////////////////////////////////////////////////////
	bool BurritoBison::testHeightEvent_( const HeightEventDesc & _desc ) const
	{
		return _desc.less ? m_position.y <= _desc.height : m_position.y >= _desc.height;
	}
}
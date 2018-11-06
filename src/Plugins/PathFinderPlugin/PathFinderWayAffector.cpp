#include "PathFinderWayAffector.h"

#include "Core/ValueInterpolator.h"

#include "pybind/pybind.hpp"

#include <math.h>

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	PathFinderWayAffector::PathFinderWayAffector()
		: m_node(nullptr)
		, m_satellite( pybind::invalid() )
		, m_way( pybind::invalid() )
		, m_speed(0.f)
		, m_offset(0.f)
		, m_length(0.f)
		, m_wayLength(0.f)
		, m_iterator(0)
		, m_wayCount(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	PathFinderWayAffector::~PathFinderWayAffector()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool PathFinderWayAffector::initialize( Node * _node, const pybind::list & _satellite, float _offset, float _speed, const pybind::list & _way, bool _preparePosition, const pybind::object & _cb, const pybind::args & _args )
	{
		if( _way.is_valid() == false )
		{
			return false;
		}

		m_node = _node;
		m_satellite = _satellite;
		m_speed = _speed;
		m_offset = _offset;

		m_way = _way;
		m_preparePosition = _preparePosition;
		m_cb = _cb;
		m_args = _args;

		m_iterator = 0;
		m_wayCount = (uint32_t)m_way.size();

		m_followerPurge.initialize( 1.f );

		if( m_wayCount < 2 )
		{
			return false;
		}

		m_wayLength = this->calcWayLength_();
				
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	Node * PathFinderWayAffector::getNode() const
	{
		return m_node;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PathFinderWayAffector::purge( float _reduce, float _speed, float _acceleration )
	{
		if( this->getFreeze() == true )
		{
			return false;
		}

		m_followerPurge.setValue( _reduce );
		m_followerPurge.setSpeed( _speed );
		m_followerPurge.setAcceleration( _acceleration );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void PathFinderWayAffector::unpurge()
	{
		m_followerPurge.setValue( 1.f );
		m_followerPurge.setSpeed( 0.f );
		m_followerPurge.setAcceleration( 0.f );
	}
	//////////////////////////////////////////////////////////////////////////
	bool PathFinderWayAffector::stepNextPoint_( const mt::vec3f & _pos, float _step, mt::vec3f & _out, mt::vec3f & _dir, uint32_t & _iterator ) const
	{
		_iterator = m_iterator;

		mt::vec3f wp = m_way[_iterator];

		float ll = mt::sqrlength_v3_v3( _pos, wp );

		if( ll > _step * _step )
		{
			mt::vec3f dir;
			mt::dir_v3_v3( dir, _pos, wp );

			_out = _pos + dir * _step;
			_dir = dir;

			return false;
		}
		
		float l = ::sqrtf( ll );

		_step -= l;

		while( ++_iterator != m_wayCount )
		{
			mt::vec3f wp_prev = m_way[_iterator - 1];
			mt::vec3f wp_current = m_way[_iterator];

			float iter_ll = mt::sqrlength_v3_v3( wp_prev, wp_current );

			if( iter_ll > _step * _step )
			{
				mt::vec3f dir;
				mt::dir_v3_v3( dir, wp_prev, wp_current );

				_out = wp_prev + dir * _step;
				_dir = dir;

				return true;
			}

			float iter_l = ::sqrtf( iter_ll );

			_step -= iter_l;
		}

		mt::vec3f wp_prev = m_way[m_wayCount - 2];
		mt::vec3f wp_current = m_way[m_wayCount - 1];

		mt::vec3f dir;
		mt::dir_v3_v3( dir, wp_prev, wp_current );

		_out = wp_current;
		_dir = dir;
				
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PathFinderWayAffector::_prepare()
	{
		mt::vec3f wp_current = m_way[0];
		//mt::vec3f wp_target = m_way[1];

		m_iterator = 1;

		if( m_preparePosition == true )
		{
			this->updatePosition_( wp_current );
		}

		mt::vec3f new_pos;
		mt::vec3f new_dir;
		this->step_( m_offset, new_pos, new_dir );
		
		if( m_cb.is_callable() == true )
		{
			uint32_t id = this->getId();

			m_cb.call_args( id, m_node, new_pos, new_dir, true, false, false, m_args );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PathFinderWayAffector::_affect( float _current, float _timing )
	{
		m_followerPurge.update( _current, _timing );

		float purge = m_followerPurge.getValue();

		float step = m_speed * purge * _timing;

		mt::vec3f new_pos;
		mt::vec3f new_dir;
		bool newTarget = this->step_( step, new_pos, new_dir );

		if( newTarget == true )
		{
			if( m_cb.is_callable() == true )
			{
				uint32_t id = this->getId();

				m_cb.call_args( id, m_node, new_pos, new_dir, false, false, false, m_args );
			}
		}

		if( m_iterator == m_wayCount )
		{
			return true;
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PathFinderWayAffector::step_( float _length, mt::vec3f & _pos, mt::vec3f & _dir )
	{
		const mt::vec3f & lp = m_node->getLocalPosition();

		mt::vec3f new_pos;
		mt::vec3f new_dir;
		uint32_t new_iterator;
		bool newTarget = this->stepNextPoint_( lp, _length, new_pos, new_dir, new_iterator );

		this->updatePosition_( new_pos );

		m_length += _length;

		_pos = new_pos;
		_dir = new_dir;

		m_iterator = new_iterator;
		
		return newTarget;
	}
	//////////////////////////////////////////////////////////////////////////
	float PathFinderWayAffector::calcWayLength_() const
	{ 
		float way_length = 0.f;

		for( uint32_t i = 0; i != (m_wayCount - 1); ++i )
		{
			mt::vec3f wp_0 = m_way[i + 0];
			mt::vec3f wp_1 = m_way[i + 1];

			float length = mt::length_v3_v3( wp_0, wp_1 );

			way_length += length;
		}

		return way_length;
	}
	//////////////////////////////////////////////////////////////////////////
	void PathFinderWayAffector::updatePosition_( const mt::vec3f & _pos )
	{
		const mt::vec3f & current_pos = m_node->getLocalPosition();

		mt::vec3f velocity = _pos - current_pos;

		m_node->translate( velocity );

        for( pybind::list::size_type
            it = 0,
            it_end = m_satellite.size();
            it != it_end;
            ++it )
		{
			Node * satellite = m_satellite[it];

			satellite->translate( velocity );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	float PathFinderWayAffector::getLength() const
	{
		return m_length;
	}
	//////////////////////////////////////////////////////////////////////////
	float PathFinderWayAffector::getWayLength() const
	{
		return m_wayLength;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & PathFinderWayAffector::getLocalPosition() const
	{ 
		const mt::vec3f & pos = m_node->getLocalPosition();

		return pos;
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec3f PathFinderWayAffector::getTimePosition( float _time ) const
	{
		const mt::vec3f & lp = m_node->getLocalPosition();

		if( this->getFreeze() == true )
		{
			return lp;
		}

		float speedFactor = this->getSpeedFactor();

		float step = m_speed * speedFactor * _time;

		mt::vec3f new_pos;
		mt::vec3f new_dir;
		uint32_t new_iterator;
		this->stepNextPoint_( lp, step, new_pos, new_dir, new_iterator );

		return new_pos;
	}
	//////////////////////////////////////////////////////////////////////////
	pybind::tuple PathFinderWayAffector::predictionLinearBullet( pybind::kernel_interface * _kernel, const mt::vec3f & _offset, const mt::vec3f & _position, float _speed ) const
	{
		mt::vec3f unit_start_position = m_node->getLocalPosition();
		unit_start_position += _offset;

		float length = mt::length_v3_v3( _position, unit_start_position );
		float time = (length / _speed) * 1000.f;

		const uint32_t max_iteration = 10;

		float unit_time = 0.f;
		mt::vec3f target_position( 0.f, 0.f, 0.f );
		float bullet_time = 0.f;

		const float dt = time / float( max_iteration );

		for( uint32_t index = 1; index != max_iteration; ++index )
		{
			float test_time = unit_time + dt;

			mt::vec3f test_target_position = this->getTimePosition( test_time );
			test_target_position += _offset;

			float dlength = mt::length_v3_v3( _position, test_target_position );
			float test_bullet_time = (dlength / _speed) * 1000.f;

			if( test_time >= test_bullet_time )
			{
				break;
			}

			unit_time = test_time;
			bullet_time = test_bullet_time;
			target_position = test_target_position;
		}

		const float dt2 = dt / float( max_iteration );

		for( uint32_t index = 1; index != max_iteration; ++index )
		{
			float test_time = unit_time + dt2;

			mt::vec3f test_target_position = this->getTimePosition( unit_time );
			test_target_position += _offset;

			float dlength = mt::length_v3_v3( _position, test_target_position );
			float test_bullet_time = (dlength / _speed) * 1000.f;

			if( test_time >= test_bullet_time )
			{
				break;
			}

			unit_time = test_time;
			bullet_time = test_bullet_time;
			target_position = test_target_position;
		}

		float delay = unit_time - bullet_time;

		return pybind::make_tuple_t( _kernel, unit_time, delay, target_position );
	}
	//////////////////////////////////////////////////////////////////////////
	pybind::tuple PathFinderWayAffector::predictionParabolicBullet( pybind::kernel_interface * _kernel, const mt::vec3f & _offset, const mt::vec3f & _position, const mt::vec3f & _height, float _speed ) const
	{
		mt::vec3f unit_start_position = m_node->getLocalPosition();
		unit_start_position += _offset;

		mt::vec3f parabolic_begin = _position;
		mt::vec3f parabolic_end = unit_start_position;
		mt::vec3f parabolic_height = (_position + unit_start_position) * 0.5f + _height;
		
		float length = calculateParabolicLength( parabolic_begin, parabolic_end, parabolic_height, mt::length_v3_v3 );
		float time = (length / _speed) * 1000.f;

		const uint32_t max_iteration = 10;

		float unit_time = 0.f;
		mt::vec3f target_position( 0.f, 0.f, 0.f );
		float bullet_time = 0.f;

		const float dt = time / float( max_iteration );

		for( uint32_t index = 1; index != max_iteration; ++index )
		{
			float test_time = unit_time + dt;

			mt::vec3f test_target_position = this->getTimePosition( test_time );
			test_target_position += _offset;

			mt::vec3f parabolic_begin_dt = _position;
			mt::vec3f parabolic_end_dt = target_position;
			mt::vec3f parabolic_height_dt = (_position + test_target_position) * 0.5f + _height;

			float dlength = calculateParabolicLength( parabolic_begin_dt, parabolic_end_dt, parabolic_height_dt, mt::length_v3_v3 );
			float test_bullet_time = (dlength / _speed) * 1000.f;

			if( test_time >= test_bullet_time )
			{
				break;
			}

			unit_time = test_time;
			bullet_time = test_bullet_time;
			target_position = test_target_position;
		}

		const float dt2 = dt / float( max_iteration );

		for( uint32_t index = 1; index != max_iteration; ++index )
		{
			float test_time = unit_time + dt2;
			
			mt::vec3f test_target_position = this->getTimePosition( test_time );
			test_target_position += _offset;

			mt::vec3f parabolic_begin_it = _position;
			mt::vec3f parabolic_end_it = target_position;
			mt::vec3f parabolic_height_it = (_position + test_target_position) * 0.5f + _height;

			float dlength = calculateParabolicLength( parabolic_begin_it, parabolic_end_it, parabolic_height_it, mt::length_v3_v3 );
			float test_bullet_time = (dlength / _speed) * 1000.f;

			if( test_time >= test_bullet_time )
			{
				break;
			}

			unit_time = test_time;
			bullet_time = test_bullet_time;
			target_position = test_target_position;
		}

		float delay = unit_time > bullet_time ? unit_time - bullet_time : 0.f;

		return pybind::make_tuple_t( _kernel, unit_time, delay, target_position );
	}
	//////////////////////////////////////////////////////////////////////////
	void PathFinderWayAffector::complete()
	{
		if( m_cb.is_callable() == true )
		{
			mt::vec3f wp_prev = m_way[m_wayCount - 2];
			mt::vec3f wp_current = m_way[m_wayCount - 1];

			mt::vec3f dir;
			mt::dir_v3_v3( dir, wp_prev, wp_current );


			uint32_t id = this->getId();

			m_cb.call_args( id, m_node, wp_current, dir, false, false, true, m_args );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void PathFinderWayAffector::stop()
	{
		if( m_cb.is_callable() == true )
		{
			const mt::vec3f & lp = m_node->getLocalPosition();

			mt::vec3f wp_prev = m_way[m_iterator - 1];
			mt::vec3f wp_current = m_way[m_iterator];

			mt::vec3f dir;
			mt::dir_v3_v3( dir, wp_prev, wp_current );

			uint32_t id = this->getId();

			m_cb.call_args( id, m_node, lp, dir, false, true, false, m_args );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void PathFinderWayAffector::_setFreeze( bool _value )
	{
		(void)_value;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * PathFinderWayAffector::_embedded()
	{
		pybind::kernel_interface * kernel = pybind::get_kernel();

		PyObject * py_obj = kernel->scope_create_holder_t( this );

		return py_obj;
	}
}


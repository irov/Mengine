#	include "PathFinderWayAffector.h"

#	include "pybind/pybind.hpp"

#	include <math.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	PathFinderWayAffector::PathFinderWayAffector()
		: m_serviceProvider(nullptr)
		, m_node(nullptr)
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
	void PathFinderWayAffector::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PathFinderWayAffector::initialize( Node * _node, float _offset, float _speed, const pybind::list & _way, const pybind::object & _cb )
	{
		if( _cb.is_callable() == false )
		{
			return false;
		}

		m_node = _node;
		m_speed = _speed;
		m_offset = _offset;

		m_way = _way;
		m_cb = _cb;

		m_iterator = 0;
		m_wayCount = m_way.size();

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
	bool PathFinderWayAffector::stepNextPoint_( const mt::vec3f & _pos, float _step, mt::vec3f & _out, mt::vec3f & _dir, uint32_t & _iterator ) const
	{
		_iterator = m_iterator;

		mt::vec3f wp = m_way[_iterator];

		float ll = mt::sqrlength_v3_v3( _pos, wp );

		if( ll > _step * _step )
		{
			mt::vec3f dir;
			mt::dir_v3_v3( dir, wp, _pos );

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
				mt::dir_v3_v3( dir, wp_current, wp_prev );

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
		mt::dir_v3_v3( dir, wp_current, wp_prev );

		_out = wp_current;
		_dir = dir;
				
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PathFinderWayAffector::prepare()
	{
		mt::vec3f wp_current = m_way[0];
		//mt::vec3f wp_target = m_way[1];

		m_iterator = 1;

		m_node->setLocalPosition( wp_current );

		mt::vec3f new_pos;
		mt::vec3f new_dir;
		this->step_( m_offset, new_pos, new_dir );
				
		uint32_t id = this->getId();

		m_cb( id, m_node, new_pos, new_dir, true, false, false );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PathFinderWayAffector::_affect( float _timing )
	{
		float step = m_speed * _timing;

		mt::vec3f new_pos;
		mt::vec3f new_dir;
		bool newTarget = this->step_( step, new_pos, new_dir );

		if( newTarget == true )
		{
			uint32_t id = this->getId();

			m_cb( id, m_node, new_pos, new_dir, false, false, false );
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

		m_node->setLocalPosition( new_pos );

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
		float step = m_speed * m_speedAffector * _time;

		const mt::vec3f & lp = m_node->getLocalPosition();

		mt::vec3f new_pos;
		mt::vec3f new_dir;
		uint32_t new_iterator;
		this->stepNextPoint_( lp, step, new_pos, new_dir, new_iterator );

		return new_pos;
	}
	//////////////////////////////////////////////////////////////////////////
	void PathFinderWayAffector::complete()
	{
		mt::vec3f wp_prev = m_way[m_wayCount - 2];
		mt::vec3f wp_current = m_way[m_wayCount - 1];

		mt::vec3f dir;
		mt::dir_v3_v3( dir, wp_current, wp_prev );

		uint32_t id = this->getId();

		m_cb( id, m_node, wp_current, dir, false, false, true );
	}
	//////////////////////////////////////////////////////////////////////////
	void PathFinderWayAffector::stop()
	{
		const mt::vec3f & lp = m_node->getLocalPosition();

		mt::vec3f wp_prev = m_way[m_iterator - 1];
		mt::vec3f wp_current = m_way[m_iterator];

		mt::vec3f dir;
		mt::dir_v3_v3( dir, wp_current, wp_prev );

		uint32_t id = this->getId();

		m_cb( id, m_node, lp, dir, false, true, false );
	}
}


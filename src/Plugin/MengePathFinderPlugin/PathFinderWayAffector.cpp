#	include "PathFinderWayAffector.h"

#	include "pybind/pybind.hpp"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	PathFinderWayAffector::PathFinderWayAffector()
		: m_serviceProvider(nullptr)
		, m_node(nullptr)
		, m_speed(0.f)
		, m_step(0.f)
		, m_cb(nullptr)
		, m_iterator(0)
		, m_target(0.f, 0.f)
		, m_length(0.f)
		, m_timing(0.f)
		, m_targetInvalidate(true)
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
	void PathFinderWayAffector::initialize( Node * _node, const PathFinderWayPtr & _way, float _speed, float _step, PyObject * _cb )
	{
		m_node = _node;
		m_way = _way;
		m_speed = _speed;
		m_step = _step;

		m_cb = _cb;
		pybind::incref( m_cb );
	}
	//////////////////////////////////////////////////////////////////////////
	bool PathFinderWayAffector::affect( float _timing )
	{
		if( m_targetInvalidate == true )
		{
			if( this->invalidateTarget_() == true )
			{
				return true;
			}
		}

		mt::vec3f lp = m_node->getLocalPosition();
		
		while( true, true )
		{
			m_timing += _timing;

			if( m_timing * m_speed <= m_length )
			{
				break;
			}			

			_timing = (m_length - m_timing * m_speed) / m_speed;

			lp.x = m_target.x;
			lp.y = m_target.y;
			lp.z = lp.z;

			m_node->setLocalPosition( lp );

			if( this->invalidateTarget_() == true )
			{
				return true;
			}
		}
		
		mt::vec2f lp_v2 = lp.to_vec2f();

		mt::vec2f dir;
		mt::sub_v2_v2( dir, m_target, lp_v2 );

		mt::vec2f dir_n;
		mt::norm_v2_v2( dir_n, dir );

		mt::vec2f dir_n_s;
		mt::mul_v2_f( dir_n_s, dir_n, m_speed * _timing );

		mt::vec2f new_position;
		mt::add_v2_v2( new_position, lp_v2, dir_n_s );

		mt::vec3f new_position_v3(new_position.x, new_position.y, lp.z);
		m_node->setLocalPosition( new_position_v3 );

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void PathFinderWayAffector::complete()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void PathFinderWayAffector::stop()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	bool PathFinderWayAffector::invalidateTarget_()
	{
		m_targetInvalidate = false;

		const mt::vec3f & lp = m_node->getLocalPosition();

		mt::vec2f lp_v2 = lp.to_vec2f();

		mt::vec2f acc(0.f, 0.f);
		size_t j = 3;

		size_t wayCount = m_way->getWayPointCount();

		if( m_iterator == wayCount )
		{
			return true;
		}

		for( size_t i = m_iterator; i != wayCount && j != 0; ++i )
		{
			const mt::vec2f & v = m_way->getWayPoint( i );

			mt::vec2f dir;
			mt::sub_v2_v2( dir, v, lp_v2 );

			mt::vec2f dir_n;
			mt::norm_v2_v2( dir_n, dir );

			acc += dir_n / (float)(i + 1);

			--j;
		}

		mt::vec2f acc_n;
		mt::norm_v2_v2( acc_n, acc );

		m_target = lp_v2 + acc_n * m_step;
		m_length = m_step;
		
		m_timing = 0.f;
		++m_iterator;

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void PathFinderWayAffector::destroy()
	{
		delete this;
	}
}


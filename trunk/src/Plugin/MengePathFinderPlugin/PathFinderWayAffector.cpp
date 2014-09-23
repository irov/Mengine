#	include "PathFinderWayAffector.h"

#	include "pybind/pybind.hpp"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	PathFinderWayAffector::PathFinderWayAffector()
		: m_serviceProvider(nullptr)
		, m_node(nullptr)
		, m_speed(0.f)
		, m_way(nullptr)
		, m_cb(nullptr)
		, m_iterator(1)
		, m_target(0.f, 0.f)
		, m_length(0.f)
		, m_timing(0.f)
		, m_targetInvalidate(true)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	PathFinderWayAffector::~PathFinderWayAffector()
	{
		pybind::decref( m_cb );
		pybind::decref( m_way );
	}
	//////////////////////////////////////////////////////////////////////////
	void PathFinderWayAffector::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PathFinderWayAffector::initialize( Node * _node, PyObject * _way, float _speed, PyObject * _cb )
	{
		if( pybind::is_callable( _cb ) == false )
		{
			return false;
		}

		m_node = _node;

		m_way = _way;
		pybind::incref( m_way );

		m_speed = _speed;
		
		m_cb = _cb;
		pybind::incref( m_cb );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PathFinderWayAffector::affect( float _timing )
	{
		bool newTarget = false;

		if( m_targetInvalidate == true )
		{
			newTarget = true;

			if( this->invalidateTarget_() == true )
			{
				return true;
			}
		}

		mt::vec3f lp = m_node->getLocalPosition();
		
		m_timing += _timing;
				
		if( m_timing * m_speed > m_length )
		{
			newTarget = true;

			do
			{
				m_timing -= m_length / m_speed;

				lp.x = m_target.x;
				lp.y = m_target.y;
				lp.z = lp.z;

				m_node->setLocalPosition( lp );

				if( this->invalidateTarget_() == true )
				{	
					return true;
				}
			}
			while( m_timing * m_speed > m_length );

			_timing = m_timing;
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

		//printf("%f %f\n", new_position.x, new_position.y);
		mt::vec3f new_position_v3(new_position.x, new_position.y, lp.z);
		m_node->setLocalPosition( new_position_v3 );

		if( newTarget == true )
		{
			uint32_t id = this->getId();
			pybind::call( m_cb, "iOffffO", id, m_node->getEmbed(), new_position.x, new_position.y, m_target.x, m_target.y, pybind::get_bool(false) );
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void PathFinderWayAffector::complete()
	{
		const mt::vec3f & lp = m_node->getLocalPosition();
		mt::vec3f new_pos(m_target.x, m_target.y, lp.z);
		m_node->setLocalPosition( new_pos );

		uint32_t id = this->getId();
		pybind::call( m_cb, "iOffffO", id, m_node->getEmbed(), lp.x, lp.y, m_target.x, m_target.y, pybind::get_bool(true) );

		pybind::decref(m_cb);
		m_cb = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	void PathFinderWayAffector::stop()
	{
		const mt::vec3f & lp = m_node->getLocalPosition();

		uint32_t id = this->getId();
		pybind::call( m_cb, "iOffffO", id, m_node->getEmbed(), lp.x, lp.y, m_target.x, m_target.y, pybind::get_bool(true) );

		pybind::decref(m_cb);
		m_cb = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PathFinderWayAffector::invalidateTarget_()
	{
		m_targetInvalidate = false;

		const mt::vec3f & lp = m_node->getLocalPosition();

		mt::vec2f lp_v2 = lp.to_vec2f();

		mt::vec2f acc_dir(0.f, 0.f);
		uint32_t j = 4;

		uint32_t wayCount = pybind::list_size( m_way );	

		if( m_iterator == wayCount )
		{
			PyObject * py_to = pybind::list_getitem( m_way, wayCount - 1 );

			mt::vec2f v_to;
			pybind::extract_value( py_to, v_to );

			m_target = v_to;

			return true;
		}

		PyObject * py_v0 = pybind::list_getitem( m_way, m_iterator );

		mt::vec2f v0;
		pybind::extract_value( py_v0, v0 );

		for( uint32_t i = m_iterator; i != wayCount && j != 0; ++i )
		{
			PyObject * py_v = pybind::list_getitem( m_way, i );

			mt::vec2f v;
			pybind::extract_value( py_v, v );			

			mt::vec2f dir;
			mt::sub_v2_v2( dir, v, lp_v2 );

			mt::vec2f dir_n;
			mt::norm_v2_v2( dir_n, dir );

			acc_dir += dir_n / (float)(i + 1);

			--j;
		}

		mt::vec2f acc_dir_n;
		mt::norm_v2_v2( acc_dir_n, acc_dir );

		mt::vec2f acc_pos;
		mt::add_v2_v2(acc_pos, lp_v2, acc_dir_n);

		mt::vec2f v0_dir;
		mt::sub_v2_v2( v0_dir, v0, lp_v2 );

		mt::vec2f step;
		mt::project_v2_v2( step, acc_dir_n, v0_dir );

		m_target = lp_v2 + step;
		m_length = mt::length_v2_v2(lp_v2, m_target);
		
		++m_iterator;

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void PathFinderWayAffector::destroy()
	{
		delete this;
	}
}


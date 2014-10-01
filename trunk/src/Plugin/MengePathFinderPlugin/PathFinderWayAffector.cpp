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
		, m_iterator(0)
		, m_wayCount(0)
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

		m_iterator = 1;
		m_wayCount = pybind::list_size( m_way );

		if( m_wayCount < 2 )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PathFinderWayAffector::prepare()
	{
		const mt::vec3f & lp = m_node->getLocalPosition();

		PyObject * py_wp = pybind::list_getitem( m_way, 1 );

		mt::vec2f wp;
		if( pybind::extract_value( py_wp, wp ) == false )
		{
			return false;
		}

		uint32_t id = this->getId();
		pybind::call( m_cb, "iOffffOOO", id, m_node->getEmbed(), lp.x, lp.y, wp.x, wp.y, pybind::get_bool(true), pybind::get_bool(false), pybind::get_bool(false) );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PathFinderWayAffector::affect( float _timing )
	{
		float step = m_speed * _timing;

		bool newTarget = false;
		
		for( ; m_iterator != m_wayCount; ++m_iterator )
		{
			PyObject * py_wp = pybind::list_getitem( m_way, m_iterator );

			mt::vec2f wp;
			pybind::extract_value( py_wp, wp );

			const mt::vec3f & lp3 = m_node->getLocalPosition();
			mt::vec2f lp = lp3.to_vec2f();			

			mt::vec2f distance;
			mt::sub_v2_v2( distance, wp, lp );
			
			float length = mt::length_v2( distance );

			if( length > step )
			{
				mt::vec2f dir;
				dir = distance / length;

				mt::vec2f new_pos;
				new_pos = lp + dir * step;

				mt::vec3f new_pos3;
				new_pos3.x = new_pos.x;
				new_pos3.y = new_pos.y;
				new_pos3.z = lp3.z;

				m_node->setLocalPosition( new_pos3 );

				if( newTarget == true )
				{
					uint32_t id = this->getId();
					pybind::call( m_cb, "iOffffOOO", id, m_node->getEmbed(), lp.x, lp.y, wp.x, wp.y, pybind::get_bool(false), pybind::get_bool(false), pybind::get_bool(false) );
				}

				return false;
			}
			else
			{
				step -= length;

				mt::vec3f new_pos3;
				new_pos3.x = wp.x;
				new_pos3.y = wp.y;
				new_pos3.z = lp3.z;

				m_node->setLocalPosition( new_pos3 );

				newTarget = true;
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void PathFinderWayAffector::complete()
	{
		const mt::vec3f & lp = m_node->getLocalPosition();

		uint32_t id = this->getId();
		pybind::call( m_cb, "iOffffOOO", id, m_node->getEmbed(), lp.x, lp.y, lp.x, lp.y, pybind::get_bool(false), pybind::get_bool(false), pybind::get_bool(true) );
	}
	//////////////////////////////////////////////////////////////////////////
	void PathFinderWayAffector::stop()
	{
		const mt::vec3f & lp = m_node->getLocalPosition();

		uint32_t id = this->getId();
		pybind::call( m_cb, "iOffffOOO", id, m_node->getEmbed(), lp.x, lp.y, lp.x, lp.y, pybind::get_bool(false), pybind::get_bool(true), pybind::get_bool(false) );
	}
	//////////////////////////////////////////////////////////////////////////
	void PathFinderWayAffector::destroy()
	{
		delete this;
	}
}


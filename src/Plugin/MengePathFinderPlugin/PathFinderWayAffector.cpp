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
	bool PathFinderWayAffector::initialize( Node * _node, float _speed, PyObject * _way, PyObject * _cb )
	{
		if( pybind::is_callable( _cb ) == false )
		{
			return false;
		}

		if( pybind::list_check( _way ) == false )
		{
			return false;
		}

		m_node = _node;
		m_speed = _speed;

		m_way = _way;
		pybind::incref( m_way );		
		
		m_cb = _cb;
		pybind::incref( m_cb );

		m_iterator = 0;
		m_wayCount = pybind::list_size( m_way );

		if( m_wayCount < 2 )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PathFinderWayAffector::stepNextPoint_( const mt::vec3f & _pos, float _step, mt::vec3f & _out, mt::vec3f & _dir )
	{
		mt::vec3f wp = pybind::list_getitem_t<mt::vec3f>( m_way, m_iterator );

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

		while( ++m_iterator != m_wayCount )
		{
			mt::vec3f wp_prev = pybind::list_getitem_t<mt::vec3f>( m_way, m_iterator - 1 );
			mt::vec3f wp_current = pybind::list_getitem_t<mt::vec3f>( m_way, m_iterator );

			float ll = mt::sqrlength_v3_v3( wp_prev, wp_current );

			if( ll > _step * _step )
			{
				mt::vec3f dir;
				mt::dir_v3_v3( dir, wp_current, wp_prev );

				_out = wp_prev + dir * _step;
				_dir = dir;

				return true;
			}

			float l = ::sqrtf( ll );

			_step -= l;
		}

		mt::vec3f wp_prev = pybind::list_getitem_t<mt::vec3f>( m_way, m_wayCount - 2 );
		mt::vec3f wp_current = pybind::list_getitem_t<mt::vec3f>( m_way, m_wayCount - 1 );

		mt::vec3f dir;
		mt::dir_v3_v3( dir, wp_current, wp_prev );

		_out = wp_current;
		_dir = dir;
				
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PathFinderWayAffector::prepare()
	{
		mt::vec3f wp_current = pybind::list_getitem_t<mt::vec3f>( m_way, 0 );
		mt::vec3f wp_target = pybind::list_getitem_t<mt::vec3f>( m_way, 1 );

		m_iterator = 1;
		
		mt::vec3f dir;
		mt::dir_v3_v3( dir, wp_target, wp_current );

		m_node->setLocalPosition( wp_current );
		
		uint32_t id = this->getId();

		pybind::call( m_cb, "iONNOOO"
			, id
			, m_node->getEmbed()
			, pybind::ptr(wp_current)
			, pybind::ptr(dir)
			, pybind::get_bool(true)
			, pybind::get_bool(false)
			, pybind::get_bool(false) 
			);

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PathFinderWayAffector::affect( float _timing )
	{
		float step = m_speed * _timing;

		const mt::vec3f & lp = m_node->getLocalPosition();

		mt::vec3f new_pos;
		mt::vec3f new_dir;
		bool newTarget = this->stepNextPoint_( lp, step, new_pos, new_dir );

		m_node->setLocalPosition( new_pos );

		if( newTarget == true )
		{
			uint32_t id = this->getId();

			pybind::call( m_cb, "iONNOOO"
				, id
				, m_node->getEmbed()
				, pybind::ptr(new_pos)
				, pybind::ptr(new_dir)
				, pybind::get_bool(false)
				, pybind::get_bool(false)
				, pybind::get_bool(false) 
				);
		}

		if( m_iterator == m_wayCount )
		{
			return true;
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void PathFinderWayAffector::complete()
	{
		mt::vec3f wp_prev = pybind::list_getitem_t<mt::vec3f>( m_way, m_wayCount - 2 );
		mt::vec3f wp_current = pybind::list_getitem_t<mt::vec3f>( m_way, m_wayCount - 1 );

		mt::vec3f dir;
		mt::dir_v3_v3( dir, wp_current, wp_prev );

		uint32_t id = this->getId();

		pybind::call( m_cb, "iONNOOO"
			, id
			, m_node->getEmbed()
			, pybind::ptr(wp_current)
			, pybind::ptr(dir)
			, pybind::get_bool(false)
			, pybind::get_bool(false)
			, pybind::get_bool(true) 
			);
	}
	//////////////////////////////////////////////////////////////////////////
	void PathFinderWayAffector::stop()
	{
		const mt::vec3f & lp = m_node->getLocalPosition();

		mt::vec3f wp_prev = pybind::list_getitem_t<mt::vec3f>( m_way, m_iterator - 1 );
		mt::vec3f wp_current = pybind::list_getitem_t<mt::vec3f>( m_way, m_iterator );

		mt::vec3f dir;
		mt::dir_v3_v3( dir, wp_current, wp_prev );

		uint32_t id = this->getId();

		pybind::call( m_cb, "iONNOOO"
			, id
			, m_node->getEmbed()
			, pybind::ptr(lp)
			, pybind::ptr(dir)
			, pybind::get_bool(false)
			, pybind::get_bool(true)
			, pybind::get_bool(false) 
			);
	}
}


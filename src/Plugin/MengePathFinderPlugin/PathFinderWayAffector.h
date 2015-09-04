#	pragma once

#	include "Kernel/Affector.h"
#	include "Kernel/Node.h"

#	include "Math/vec2.h"

#	include "fastpathfinder/pathfinder.h"
#	include "fastpathfinder/map.h"

#	include "pybind/pybind.hpp"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class PathFinderWayAffector
		: public Affector
	{
	public:
		PathFinderWayAffector();
		~PathFinderWayAffector();
		
	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider );

	public:
		bool initialize( Node * _node, float _speed, const pybind::list & _way, const pybind::object & _cb );
		
	public:
		bool prepare() override;
		bool affect( float _timing ) override;
		void complete() override;
		void stop() override;

	protected:
		bool stepNextPoint_( const mt::vec3f & _pos, float _step, mt::vec3f & _out, mt::vec3f & _dir );
		bool invalidateTarget_();

	protected:
		ServiceProviderInterface * m_serviceProvider;

		Node * m_node;
		pybind::list m_way;

		float m_speed;
		pybind::object m_cb;

		uint32_t m_iterator;
		uint32_t m_wayCount;
	};
}


#	pragma once

#	include "Kernel/Affector.h"
#	include "Kernel/Node.h"

#	include "Math/vec2.h"

#	include "fastpathfinder/pathfinder.h"
#	include "fastpathfinder/map.h"

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
		bool initialize( Node * _node, float _speed, PyObject * _way, PyObject * _cb );
		
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
		PyObject * m_way;

		float m_speed;
		PyObject * m_cb;

		uint32_t m_iterator;
		uint32_t m_wayCount;
	};
}


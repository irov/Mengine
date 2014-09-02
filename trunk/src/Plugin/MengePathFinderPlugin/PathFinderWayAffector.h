#	pragma once

#	include "Kernel/Affector.h"
#	include "Kernel/Node.h"

#	include "PathFinderWay.h"

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
		void initialize( Node * _node, const PathFinderWayPtr & _way, float _speed, float _step, PyObject * _cb );
		
	public:
		bool affect( float _timing ) override;
		void complete() override;
		void stop() override;

	protected:
		bool invalidateTarget_();

	protected:
		void destroy() override;

	protected:
		ServiceProviderInterface * m_serviceProvider;

		Node * m_node;
		PathFinderWayPtr m_way;
		float m_speed;
		float m_step;
		PyObject * m_cb;

		size_t m_iterator;
		mt::vec2f m_target;
		float m_length;
		float m_timing;
		bool m_targetInvalidate;		
	};
}


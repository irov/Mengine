#	pragma once

#	include "Interface/RenderSystemInterface.h"

#	include "poly2tri.h"

#	include "Math/vec2.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	typedef std::vector<Poly2Tri::Point *> TVectorPathPoint;
	//////////////////////////////////////////////////////////////////////////
	typedef std::vector<mt::vec2f> TVectorWayPoint;
	//////////////////////////////////////////////////////////////////////////
	class PathFinderWay
	{
	public:
		PathFinderWay( ServiceProviderInterface * _serviceProvider );
		~PathFinderWay();

	public:
		void initialize( const mt::vec2f & _from, const mt::vec2f & _to, const TVectorPathPoint & _path );

	public:
		void render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera );

	protected:
		ServiceProviderInterface * m_serviceProvider;

		mt::vec2f m_from;
		mt::vec2f m_to;

		TVectorWayPoint m_wayPoint;
	};
}


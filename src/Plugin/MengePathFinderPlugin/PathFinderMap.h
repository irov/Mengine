#	pragma once

#	include "Interface/ModuleInterface.h"

#	include "PathFinderWay.h"

#	include "Core/Polygon.h"

#	include "poly2tri.h"

namespace Menge
{
	class PathFinderMap
	{
	public:
		PathFinderMap( ServiceProviderInterface * _serviceProvider );
		~PathFinderMap();

	public:
		void setMap( const Polygon & _polygon );
		void setUnitSize( float _size );

	public:
		bool addHole( size_t _index, const Polygon & _polygon );
		
	public:
		bool generateMap();

	public:
		PathFinderWay * findPath( const mt::vec2f & _from, const mt::vec2f & _to );

	public:
		void render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera );

	protected:
		bool testHoles_( size_t _index, const Polygon & _polygon ) const;
		void filterWayPoints_( TVectorWayPoint & _fileter, const TVectorWayPoint & _ways );

	protected:
		ServiceProviderInterface * m_serviceProvider;

		Polygon m_mapPolygon;
		float m_unitSize;

		typedef std::map<size_t, Polygon> THoles;
		THoles m_holes;
		THoles m_bigHoles;

		Poly2Tri::Point m_cachePoint[1024];
		size_t m_cachePointUse;

		Poly2Tri::SweepContext m_sweepContext;

		typedef std::vector<PathFinderWay *> TVectorPathFinderWay;
		TVectorPathFinderWay m_pathFinderWays;
	};
}
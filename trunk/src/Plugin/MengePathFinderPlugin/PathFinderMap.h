#	pragma once

#	include "Interface/ModuleInterface.h"

#	include "PathFinderWay.h"

#	include "Core/Polygon.h"

#	include "poly2tri.h"

#	include <list>

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
		size_t addObstacle( const Polygon & _polygon );
		void removeObstacle( size_t _id );
		
	public:
		bool generateMap();

	public:
		PathFinderWay * findPath( const mt::vec2f & _from, const mt::vec2f & _to );

	public:
		void render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera );

	protected:
		bool testHoles_( const Polygon & _polygon ) const;
		void filterWayPoints_( TVectorWayPoint & _fileter, const TVectorWayPoint & _ways );

	protected:
		ServiceProviderInterface * m_serviceProvider;

		Polygon m_mapPolygon;
		float m_unitSize;

		struct Obstacle
		{
			size_t id;

			Polygon hole;
			Polygon bigHole;
		};

		size_t m_obstacleEnumerator;

		typedef std::list<Obstacle> TObstacles;
		TObstacles m_obstales;

		Poly2Tri::Point m_cachePoint[1024];
		size_t m_cachePointUse;

		Poly2Tri::SweepContext m_sweepContext;

		typedef std::vector<PathFinderWay *> TVectorPathFinderWay;
		TVectorPathFinderWay m_pathFinderWays;
	};
}
#	pragma once

#	include "Interface/ModuleInterface.h"

#	include "PathFinderWay.h"

#	include "Core/Polygon.h"

#	include "poly2tri/poly2tri.h"

#	include <list>

namespace Menge
{
	typedef std::vector<p2t::Point *> Points;
	typedef std::vector<p2t::Triangle *> Triangles;

	struct PFMPoint
		: public p2t::Point
	{
		float weight;

		typedef std::vector<PFMPoint *> TVectorNeighbor;
		TVectorNeighbor neighbor;
	};

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
		void removePath( PathFinderWay * _way );

	public:
		void setCamera2D( const RenderCameraInterface * _camera );

	public:
		void render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, unsigned int _debugMask );

	protected:
		bool testHolesPolygon_( const Polygon & _polygon ) const;
		bool testHolesSegment_( const mt::vec2f & _p0, const mt::vec2f & _p1 ) const;
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

		PFMPoint m_cachePoint[1024];
		size_t m_cachePointUse;
				
		p2t::SweepContext * m_sweepContext;

		typedef std::vector<PathFinderWay *> TVectorPathFinderWay;
		TVectorPathFinderWay m_pathFinderWays;

		const RenderCameraInterface * m_camera;
	};
}
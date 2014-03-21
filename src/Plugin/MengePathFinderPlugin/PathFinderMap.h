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

	typedef std::vector<PFMPoint *> PFMPoints;

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
		bool generateMap2();

	public:
		PathFinderWay * findPath( const mt::vec2f & _from, const mt::vec2f & _to );
		PathFinderWay * findPath2( const mt::vec2f & _from, const mt::vec2f & _to );
		void removePath( PathFinderWay * _way );

	public:
		void setCamera2D( const RenderCameraInterface * _camera );

	public:
		void render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, unsigned int _debugMask );

	protected:
		bool testBigHolesPolygon_( const Polygon & _polygon ) const;
		bool testHolesPolygon_( const Polygon & _polygon ) const;
		bool testHolesSegment_( const mt::vec2f & _p0, const mt::vec2f & _p1 ) const;
		void calcNeighborPoints_( PFMPoints & _wayPoints ) const;
		void filterWayPoints_( TVectorWayPoint & _fileter, const TVectorWayPoint & _ways );
		void attachNeighbor_( Points & _points, Points::size_type _begin, Points::size_type _end, PFMPoint * _p0 ) const;

		PFMPoint * findNearestPoint_( const mt::vec2f & _v ) const;

	protected:
		PFMPoint * createPoint_( float _x, float _y );
		PFMPoint * createPoint2_( float _x, float _y );
		bool makePolyPointFromPolygon_( const Polygon & _polygon, Points & _points );

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

		PFMPoint m_cachePoint2[1024];
		size_t m_cachePointUse2;

		Points m_points;

		p2t::SweepContext * m_sweepContext;

		typedef std::vector<PathFinderWay *> TVectorPathFinderWay;
		TVectorPathFinderWay m_pathFinderWays;

		const RenderCameraInterface * m_camera;
	};
}
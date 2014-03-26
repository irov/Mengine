#	pragma once

#	include "Interface/ModuleInterface.h"

#	include "PathFinderWay.h"

#	include "Core/Polygon.h"

#	include <list>

namespace Menge
{
	struct PFMPoint
	{
		PFMPoint()
			: v(0.f, 0.f)
			, weight(0.f)
			, neighbor_left(nullptr)
			, neighbor_right(nullptr)
		{
		}

		PFMPoint( const mt::vec2f & _v )
			: v(_v)
			, weight(0.f)
			, neighbor_left(nullptr)
			, neighbor_right(nullptr)
		{
		}

		mt::vec2f v;
		
		typedef std::vector<PFMPoint *> TVectorNeighbor;
		TVectorNeighbor neighbor_other;

		PFMPoint * neighbor_left;
		PFMPoint * neighbor_right;

		mutable float weight;
	};

	typedef std::vector<PFMPoint *> PFMPoints;

	struct Obstacle
	{
		size_t id;

		Polygon hole;
		Polygon bigHole;

		PFMPoints points;
	};

	typedef std::vector<Obstacle *> TVectorObstacles;

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
		void renderPolygonRing_( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, const Polygon::ring_type & _ring, uint32_t _color );
		void renderPointsNeighbor_( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, const PFMPoints & _points, uint32_t _color );

	protected:
		bool testBigHolesPolygon_( const Polygon & _polygon ) const;
		bool testHolesPolygon_( const Polygon & _polygon ) const;
		bool testHolesSegment_( const mt::vec2f & _p0, const mt::vec2f & _p1 ) const;
		bool testHolesPoint_( const mt::vec2f & _p ) const;
		void calcNeighborPoints_( PFMPoints & _wayPoints ) const;
		void filterWayPoints_( TVectorWayPoint & _fileter, const TVectorWayPoint & _ways );
		void attachNeighbor2_( TVectorObstacles::size_type _begin, TVectorObstacles::size_type _end, PFMPoint * _point );
		void attachNeighbor_( PFMPoint * _point );
		void fillObstaclePointWeight_( float _weight ) const;

		PFMPoint * findNearestPoint_( const mt::vec2f & _v ) const;

	protected:
		PFMPoint * createPoint_( const mt::vec2f & _v );
		PFMPoint * createPoint2_( const mt::vec2f & _v );

		bool makePolyPointFromRing_( const Polygon::ring_type & _ring, PFMPoints & _points );
		bool makePolyPointFromPolygon_( const Polygon & _polygon, PFMPoints & _points );

	protected:
		ServiceProviderInterface * m_serviceProvider;

		Polygon m_mapPolygon;
		float m_unitSize;

		size_t m_obstacleEnumerator;
	
		TVectorObstacles m_obstacles;
		bool m_invalidateMap;

		PFMPoint m_cachePoint[1024];
		size_t m_cachePointUse;

		PFMPoint m_cachePoint2[1024];
		size_t m_cachePointUse2;
			
		typedef std::vector<PathFinderWay *> TVectorPathFinderWay;
		TVectorPathFinderWay m_pathFinderWays;

		const RenderCameraInterface * m_camera;
	};
}
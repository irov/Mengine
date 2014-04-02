#	pragma once

#	include "Interface/ModuleInterface.h"

#	include "PathFinderWay.h"

#	include "Core/Polygon.h"

#	include "fastpathfinder/fastpathfinder.h"

#	include <list>

namespace Menge
{
	struct Obstacle
	{
		size_t id;

		Polygon hole;
		Polygon bigHole;
	};
	
	typedef std::vector<Obstacle *> TVectorObstacles;

	class PathFinderMap
	{
	public:
		PathFinderMap( ServiceProviderInterface * _serviceProvider );
		~PathFinderMap();

	public:
		void setMap( float _width, float _height, float _gridSize, float _unitSize );

	public:
		bool testPoint( const mt::vec2f & _polygon );
		bool testObstacle( const Polygon & _polygon );
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

	protected:
		bool testBigHolesPolygon_( const Polygon & _polygon ) const;
		bool testHolesPolygon_( const Polygon & _polygon ) const;
		bool testHolesSegment_( const mt::vec2f & _p0, const mt::vec2f & _p1 ) const;
		bool testHolesPoint_( const mt::vec2f & _p ) const;

	protected:
		ServiceProviderInterface * m_serviceProvider;

		float m_width;
		float m_height;
		float m_gridSize;
		float m_unitSize;

		fastpathfinder::map m_map;
		
		size_t m_obstacleEnumerator;
	
		TVectorObstacles m_obstacles;
		bool m_invalidateMap;
					
		typedef std::vector<PathFinderWay *> TVectorPathFinderWay;
		TVectorPathFinderWay m_pathFinderWays;

		const RenderCameraInterface * m_camera;
	};
}
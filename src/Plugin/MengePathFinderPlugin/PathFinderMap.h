#	pragma once

#	include "Interface/ModuleInterface.h"

#	include "PathFinder.h"
#	include "PathFinderWay.h"
#	include "ThreadWorkerPathFinder.h"

#	include "Core/Polygon.h"
#	include "Factory/FactoryStore.h"

#	include "pybind/pybind.hpp"

namespace Menge
{
	struct Obstacle
	{
		size_t id;

		Polygon hole;
		Polygon bigHole;
		mt::vec2f bigMinHole;
		mt::vec2f bigMaxHole;
	};
	
	typedef std::vector<Obstacle *> TVectorObstacles;
	
	class PathFinderMap
	{
	public:
		PathFinderMap();
		~PathFinderMap();

	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider );

	public:
		bool initialize();
		void update();

	public:
		void setMap( float _width, float _height, float _gridSize, float _unitSize );

	public:
		bool testPoint( const mt::vec2f & _polygon );
		bool testObstacle( const Polygon & _polygon );
		size_t addObstacle( const Polygon & _polygon );
		void removeObstacle( size_t _id );
		
	public:
		size_t createPathFinder( const mt::vec2f & _from, const mt::vec2f & _to, PyObject * _cb );
		void removePathFinder( size_t _finderId );

	protected:
		void clearPathFinderComplete_();

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
		void obstacleCellMask_( Obstacle * _obstacle, uint32_t _mask );

	public:
		struct PathFinderDesc
		{
			size_t id;
			PathFinderPtr finder;
			PyObject * cb;
			bool complete;
		};

	protected:
		ServiceProviderInterface * m_serviceProvider;

		float m_width;
		float m_height;
		float m_gridSize;
		float m_unitSize;

		TPathMap m_map;		
		
		size_t m_enumeratorObstacles;
	
		TVectorObstacles m_obstacles;
		
		typedef std::vector<PathFinderPtr> TVectorPathFinder;
		TVectorPathFinder m_pathFinders;

		//typedef std::vector<PathFinderWay *> TVectorPathFinderWay;
		//TVectorPathFinderWay m_pathFinderWays;

		typedef std::vector<PathFinderDesc> TVectorPathFinderDesc;
		TVectorPathFinderDesc m_pathfinders;		

		size_t m_enumeratorPathFinders;

		ThreadQueueInterfacePtr m_threadPathFinders;

		typedef FactoryPoolStore<PathFinder, 32> TFactoryPathFinder;
		TFactoryPathFinder m_factoryPathFinder;

		const RenderCameraInterface * m_camera;
	};
}
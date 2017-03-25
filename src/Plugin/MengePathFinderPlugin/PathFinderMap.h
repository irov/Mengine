#	pragma once

#	include "Interface/ModuleInterface.h"

#	include "PathFinder.h"
#	include "PathFinderWay.h"
#	include "ThreadWorkerPathFinder.h"

#	include "Core/Polygon.h"
#	include "Factory/Factory.h"

#	include "pybind/pybind.hpp"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	struct Obstacle
	{
		uint32_t id;

		Polygon hole;
		Polygon bigHole;
		mt::vec2f bigMinHole;
		mt::vec2f bigMaxHole;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::vector<Obstacle *> TVectorObstacles;
	//////////////////////////////////////////////////////////////////////////
	class PathFinderMap
	{
	public:
		PathFinderMap();
		~PathFinderMap();

	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider );

	public:
		bool initialize();
		void finalize();

	public:
		void update();

	public:
		void setMap( float _width, float _height, float _gridSize, float _unitSize );
		void setMapWeight( const uint8_t * _weight );

	public:
		bool testMap( const mt::vec2f & _point ) const;			

	public:
		bool testPoint( const mt::vec2f & _point );
		bool testObstacle( const Polygon & _polygon );
		uint32_t addObstacle( const Polygon & _polygon );
		void removeObstacle( uint32_t _id );
		
	public:
		uint32_t createPathFinder( const mt::vec2f & _from, const mt::vec2f & _to, const pybind::object & _cb );
		void removePathFinder( uint32_t _finderId );

	protected:
		void clearPathFinderComplete_();

	public:
		void setCamera2D( const RenderCameraInterface * _camera );

	public:
		void render( const RenderObjectState * _state, uint32_t _debugMask );

	protected:
		//void renderPolygonRing_( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, const RenderClipplaneInterface * _clipplane, const Polygon::ring_type & _ring, uint32_t _color );
		
	protected:
		bool testBigHolesPolygon_( const Polygon & _polygon ) const;
		bool testHolesPolygon_( const Polygon & _polygon ) const;
		bool testHolesSegment_( const mt::vec2f & _p0, const mt::vec2f & _p1 ) const;
		bool testHolesPoint_( const mt::vec2f & _p ) const;

	protected:
		void obstacleCellMask_( Obstacle * _obstacle, uint8_t _mask );

	public:
		struct PathFinderDesc
		{
			uint32_t id;
			PathFinderPtr finder;
			pybind::object cb;
			bool successful;
			bool complete;
		};

	protected:
		ServiceProviderInterface * m_serviceProvider;

		float m_width;
		float m_height;
		float m_gridSize;
		float m_unitSize;

		TPathMap m_map;		
		
		uint32_t m_enumeratorObstacles;
	
		TVectorObstacles m_obstacles;
		
		typedef stdex::vector<PathFinderDesc> TVectorPathFinderDesc;
		TVectorPathFinderDesc m_pathfinders;		

		uint32_t m_enumeratorPathFinders;

		ThreadQueueInterfacePtr m_threadPathFinders;

		FactoryPtr m_factoryPathFinder;

		const RenderCameraInterface * m_camera;
	};
}
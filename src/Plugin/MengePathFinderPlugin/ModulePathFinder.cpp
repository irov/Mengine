#	include "ModulePathFinder.h"

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/PlayerInterface.h"

#	include "PathFinderWay.h"

#	include "pybind/pybind.hpp"

//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ModulePathFinder::ModulePathFinder()
		: m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ModulePathFinder::~ModulePathFinder()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ModulePathFinder::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	ServiceProviderInterface * ModulePathFinder::getServiceProvider() const
	{
		return m_serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ModulePathFinder::initialize()
	{
		pybind::interface_<PathFinderMap>("PathFinderMap")
			.def( "setMap", &PathFinderMap::setMap )
			.def( "testMap", &PathFinderMap::testMap )
			.def( "testPoint", &PathFinderMap::testPoint )
			.def( "testObstacle", &PathFinderMap::testObstacle )
			.def( "addObstacle", &PathFinderMap::addObstacle )
			.def( "removeObstacle", &PathFinderMap::removeObstacle )
			//.def( "findPath", &PathFinderMap::findPath )
			//.def( "removePath", &PathFinderMap::removePath )
			.def( "setCamera2D", &PathFinderMap::setCamera2D )
			.def( "createPathFinder", &PathFinderMap::createPathFinder )
			.def( "removePathFinder", &PathFinderMap::removePathFinder )
			;

		pybind::interface_<PathFinderWay>("PathFinderWay")
			.def( "getWayPointCount", &PathFinderWay::getWayPointCount )
			.def( "getWayPoint", &PathFinderWay::getWayPoint )
			;

		pybind::def_functor( "createPathFinderMap", this, &ModulePathFinder::createMap );	
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ModulePathFinder::finalize()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ModulePathFinder::setName( const ConstString & _name )
	{
		m_name = _name;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ModulePathFinder::getName() const
	{
		return m_name;
	}
	//////////////////////////////////////////////////////////////////////////
	PathFinderMap * ModulePathFinder::createMap()
	{
		PathFinderMap * map = new PathFinderMap();

		map->setServiceProvider( m_serviceProvider );
		
		if( map->initialize() == false )
		{
			delete map;

			return nullptr;
		}

		m_maps.push_back( map );

		return map;
	}
	//////////////////////////////////////////////////////////////////////////
	void ModulePathFinder::update( float _time, float _timing )
	{
		(void)_time;
		(void)_timing;

		for( TPathFinderMaps::iterator 
			it = m_maps.begin(),
			it_end = m_maps.end();
		it != it_end;
		++it )
		{
			PathFinderMap * map = *it;

			map->update();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ModulePathFinder::render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, unsigned int _debugMask )
	{
		for( TPathFinderMaps::iterator 
			it = m_maps.begin(),
			it_end = m_maps.end();
		it != it_end;
		++it )
		{
			PathFinderMap * map = *it;

			map->render( _viewport, _camera, _debugMask );
		}
	}
}
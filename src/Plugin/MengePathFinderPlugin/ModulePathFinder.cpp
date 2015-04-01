#	include "ModulePathFinder.h"

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/StringizeInterface.h"
#	include "Interface/PlayerInterface.h"
#	include "Interface/ResourceInterface.h"

#	include "PathGraphNode.h"

#	include "Kernel/ResourceImageData.h"

#	include "Kernel/ScriptClassWrapper.h"

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
	static void * superclass_new_PathGraphNode( const pybind::class_type_scope_ptr & _scope, PyObject * _obj, PyObject * _args, PyObject * _kwds )
	{
		(void)_scope;
		(void)_obj;
		(void)_args;
		(void)_kwds;		

		//void * user = _scope->get_user();
		//ServiceProviderInterface * serviceProvider = static_cast<ServiceProviderInterface *>(user);

		PathGraphNode * node = new PathGraphNode;

		node->setEmbed( _obj );

		return node;
	}
	//////////////////////////////////////////////////////////////////////////
	static void superclass_dealloc_only_python( const pybind::class_type_scope_ptr & _scope, void * _impl )
	{
		(void)_scope;
		(void)_impl;
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	static PyObject * s_fastpathfinder_graph_getPath( fastpathfinder::graph * _graph, fastpathfinder::graph_node * _from, fastpathfinder::graph_node * _to )
	{
		fastpathfinder::vector_graph_node path;

		_graph->getPath( _from, _to, path );
		
		PyObject * py_path = pybind::list_new(0);

		for( fastpathfinder::vector_graph_node::iterator
			it = path.begin(),
			it_end = path.end();
		it != it_end;
		++it )
		{
			PathGraphNode * node = static_cast<PathGraphNode *>(*it);
			
			pybind::list_appenditem_t( py_path, node );
		}

		return py_path;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ModulePathFinder::initialize()
	{
		pybind::interface_<fastpathfinder::graph_node>("fastpathfinder::graph_node", true)
			;

		pybind::superclass_<PathGraphNode, pybind::bases<fastpathfinder::graph_node> >("PathGraphNode", (void *)m_serviceProvider, &superclass_new_PathGraphNode, &superclass_dealloc_only_python, false)
			.def_constructor( pybind::init<>() )
			.def( "getWeight", &PathGraphNode::getWeight )
			;

		pybind::interface_<fastpathfinder::graph>("PathGraph")
			.def( "addNode", &fastpathfinder::graph::addNode )
			.def( "addEdge", &fastpathfinder::graph::addEdge )
			.def( "addEdge2", &fastpathfinder::graph::addEdge2 )
			.def( "setBlock", &fastpathfinder::graph::setBlock )
			.def( "getBlock", &fastpathfinder::graph::getBlock )
			.def_static( "getPath", &s_fastpathfinder_graph_getPath )
			;

		pybind::def_functor( "createPathFinderGraph", this, &ModulePathFinder::createGraph );
		pybind::def_functor( "destroyPathFinderGraph", this, &ModulePathFinder::destroyGraph );

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


		pybind::def_functor( "createPathFinderMap", this, &ModulePathFinder::createMap );		
		pybind::def_functor( "destroyPathFinderMap", this, &ModulePathFinder::destroyMap );
		pybind::def_functor( "setPathFinderMapWeight", this, &ModulePathFinder::setMapWeight );

		pybind::interface_<PathFinderWayAffector, pybind::bases<Affector> >("PathFinderWayAffector")
			;

		pybind::def_functor( "createPathFinderWayAffertor", this, &ModulePathFinder::createPathFinderWayAffertor );

		SCRIPT_SERVICE(m_serviceProvider)
			->addWrapping( Helper::stringizeString(m_serviceProvider, "PathGraphNode"), new ScriptClassWrapper<PathGraphNode>() );
		
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
	void ModulePathFinder::destroyMap( PathFinderMap * _map )
	{
		_map->finalize();

		delete _map;
	}
	//////////////////////////////////////////////////////////////////////////
	fastpathfinder::graph * ModulePathFinder::createGraph()
	{
		return new fastpathfinder::graph;
	}
	//////////////////////////////////////////////////////////////////////////
	void ModulePathFinder::destroyGraph( fastpathfinder::graph * _graph )
	{
		delete _graph;
	}
	//////////////////////////////////////////////////////////////////////////
	PathFinderWayAffector * ModulePathFinder::createPathFinderWayAffertor( Node * _node, PyObject * _way, float _speed, PyObject * _cb )
	{
		PathFinderWayAffector * affector = m_factoryPathFinderWayAffector.createObjectT();

		if( affector->initialize( _node, _speed, _way, _cb ) == false )
		{
			return nullptr;
		}

		if( _node->addAffector( affector ) == INVALID_AFFECTOR_ID )
		{
			m_factoryPathFinderWayAffector.destroyObject( affector );

			return nullptr;
		}

		return affector;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ModulePathFinder::setMapWeight( PathFinderMap * _map, const ConstString & _resourceName )
	{		
		ResourceImageData * resource;
		if( RESOURCE_SERVICE(m_serviceProvider)
			->hasResourceT( _resourceName, &resource ) == false )
		{
			return false;
		}

		if( resource->incrementReference() == false )
		{
			return false;
		}

		unsigned char * buffer = resource->getImageBuffer();

		_map->setMapWeight( buffer );

		resource->decrementReference();

		return true;
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
#	include "ModulePathFinder.h"

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/StringizeInterface.h"
#	include "Interface/PlayerInterface.h"
#	include "Interface/ResourceInterface.h"

#	include "PathGraphNode.h"

#	include "Kernel/ResourceImageData.h"
#   include "Kernel/ScriptWrapper.h"

#	include "pybind/pybind.hpp"

//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ModulePathFinder::ModulePathFinder()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ModulePathFinder::~ModulePathFinder()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	class superclass_new_PathGraphNode
		: public pybind::new_adapter_interface
	{
	public:
		void * call( const pybind::class_type_scope_ptr & _scope, PyObject * _obj, PyObject * _args, PyObject * _kwds ) override
		{
			(void)_scope;
			(void)_obj;
			(void)_args;
			(void)_kwds;

			//void * user = _scope->get_user();
			//ServiceProviderInterface * serviceProvider = static_cast<ServiceProviderInterface *>(user);

			PathGraphNode * node = new PathGraphNode;

			node->setEmbed( _obj );

			pybind::decref( _obj );

			return node;
		}
	};
	//////////////////////////////////////////////////////////////////////////
	static pybind::list s_fastpathfinder_graph_getPath( pybind::kernel_interface * _kernel, fastpathfinder::graph * _graph, fastpathfinder::graph_node * _from, fastpathfinder::graph_node * _to, uint32_t _mask )
	{
		fastpathfinder::vector_graph_node path;

		_graph->getPath( _from, _to, _mask, path );
		
		pybind::list py_path( _kernel );

		for( fastpathfinder::vector_graph_node::iterator
			it = path.begin(),
			it_end = path.end();
		it != it_end;
		++it )
		{
			PathGraphNode * node = static_cast<PathGraphNode *>(*it);
			
			py_path.append( node );
		}

		return py_path;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ModulePathFinder::_initialize()
	{
		pybind::kernel_interface * kernel = pybind::get_kernel();

		pybind::interface_<fastpathfinder::graph_node>( kernel, "fastpathfinder::graph_node", true )
			;

		pybind::superclass_<PathGraphNode, pybind::bases<fastpathfinder::graph_node> >( kernel, "PathGraphNode", (void *)m_serviceProvider, new superclass_new_PathGraphNode, nullptr, false )
			.def_constructor( pybind::init<>() )
			.def( "getWeight", &PathGraphNode::getWeight )
			;

		pybind::interface_<fastpathfinder::graph>( kernel, "PathGraph" )
			.def( "addNode", &fastpathfinder::graph::addNode )
			.def( "addEdge", &fastpathfinder::graph::addEdge )
			.def( "addEdge2", &fastpathfinder::graph::addEdge2 )
			.def( "setBlock", &fastpathfinder::graph::setBlock )
			.def( "getBlock", &fastpathfinder::graph::getBlock )
			.def( "hasPath", &fastpathfinder::graph::hasPath )
			.def( "setMask", &fastpathfinder::graph::setMask )
			.def( "getMask", &fastpathfinder::graph::getMask )
			.def( "getPathWeight", &fastpathfinder::graph::getPathWeight )
			.def_static_kernel( "getPath", &s_fastpathfinder_graph_getPath )
			;

		pybind::def_functor( kernel, "createPathFinderGraph", this, &ModulePathFinder::createGraph );
		pybind::def_functor( kernel, "destroyPathFinderGraph", this, &ModulePathFinder::destroyGraph );

		pybind::interface_<PathFinderMap>( kernel, "PathFinderMap" )
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


		pybind::def_functor( kernel, "createPathFinderMap", this, &ModulePathFinder::createMap );
		pybind::def_functor( kernel, "destroyPathFinderMap", this, &ModulePathFinder::destroyMap );
		pybind::def_functor( kernel, "setPathFinderMapWeight", this, &ModulePathFinder::setMapWeight );

		pybind::interface_<PathFinderWayAffector, pybind::bases<Affector> >( kernel, "PathFinderWayAffector" )
			.def( "purge", &PathFinderWayAffector::purge )
			.def( "unpurge", &PathFinderWayAffector::unpurge )
			.def( "getLength", &PathFinderWayAffector::getLength )
			.def( "getWayLength", &PathFinderWayAffector::getWayLength )
			.def( "getLocalPosition", &PathFinderWayAffector::getLocalPosition )
			.def( "getTimePosition", &PathFinderWayAffector::getTimePosition )
			.def_kernel( "predictionLinearBullet", &PathFinderWayAffector::predictionLinearBullet )
			.def_kernel( "predictionParabolicBullet", &PathFinderWayAffector::predictionParabolicBullet )
			;
		
		pybind::def_functor_args( kernel, "createPathFinderWayAffector", this, &ModulePathFinder::createPathFinderWayAffector );
		pybind::def_functor( kernel, "destroyPathFinderWayAffector", this, &ModulePathFinder::destroyPathFinderWayAffector );
		
		SCRIPT_SERVICE(m_serviceProvider)
			->setWrapper( Helper::stringizeString( m_serviceProvider, "PathGraphNode" ), new ScriptWrapper<PathGraphNode>() );
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ModulePathFinder::_finalize()
	{
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
	PathFinderWayAffector * ModulePathFinder::createPathFinderWayAffector( Node * _node, const pybind::list & _satellite, const pybind::list & _way, float _offset, float _speed, bool _preparePosition, const pybind::object & _cb, const pybind::detail::args_operator_t & _args )
	{
		PathFinderWayAffector * affector = m_factoryPathFinderWayAffector.createObject();

		affector->setServiceProvider( m_serviceProvider );

		if( affector->initialize( _node, _satellite, _offset, _speed, _way, _preparePosition, _cb, _args ) == false )
		{
			affector->destroy();

			return nullptr;
		}

		if( _node->addAffector( affector ) == INVALID_AFFECTOR_ID )
		{
			affector->destroy();

			return nullptr;
		}

		return affector;
	}
	//////////////////////////////////////////////////////////////////////////
	void ModulePathFinder::destroyPathFinderWayAffector( PathFinderWayAffector * _affector )
	{
		Node * node = _affector->getNode();

		uint32_t id = _affector->getId();

		node->stopAffector( id );
	}
	//////////////////////////////////////////////////////////////////////////
	bool ModulePathFinder::setMapWeight( PathFinderMap * _map, const ConstString & _resourceName )
	{		
		ResourceImageDataPtr resource;
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
	void ModulePathFinder::_tick( float _time, float _timing )
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
	void ModulePathFinder::_render( const RenderObjectState * _state, unsigned int _debugMask )
	{
		for( TPathFinderMaps::iterator 
			it = m_maps.begin(),
			it_end = m_maps.end();
		it != it_end;
		++it )
		{
			PathFinderMap * map = *it;

			map->render( _state, _debugMask );
		}
	}
}
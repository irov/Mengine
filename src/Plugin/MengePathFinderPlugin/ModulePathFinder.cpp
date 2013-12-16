#	include "ModulePathFinder.h"

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/PlayerInterface.h"

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
			.def( "addHole", &PathFinderMap::addHole )
			.def( "generateMap", &PathFinderMap::generateMap )
			.def( "findPath", &PathFinderMap::findPath )
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
		PathFinderMap * map = new PathFinderMap;

		m_maps.push_back( map );

		return map;
	}
	//////////////////////////////////////////////////////////////////////////
	void ModulePathFinder::update( float _time, float _timing )
	{
		(void)_time;
		(void)_timing;
	}
	//////////////////////////////////////////////////////////////////////////
	void ModulePathFinder::render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera )
	{
		//const Poly2Tri::TVectorTriangle & triangles = m_cdt->GetTriangles();

		//for( Poly2Tri::TVectorTriangle::const_iterator
		//	it = triangles.begin(),
		//	it_end = triangles.end();
		//it != it_end;
		//++it )
		//{
		//	Poly2Tri::Triangle * tr = *it;

		//	const Poly2Tri::Point * p0 = tr->GetPoint(0);
		//	const Poly2Tri::Point * p1 = tr->GetPoint(1);
		//	const Poly2Tri::Point * p2 = tr->GetPoint(2);

		//	RenderVertex2D * vertices = RENDER_SERVICE(m_serviceProvider)
		//		->getDebugRenderVertex2D( 3 * 2 );

		//	vertices[0].pos.x = p0->x;
		//	vertices[0].pos.y = p0->y;
		//	vertices[0].pos.z = 0.f;

		//	vertices[1].pos.x = p1->x;
		//	vertices[1].pos.y = p1->y;
		//	vertices[1].pos.z = 0.f;


		//	vertices[2].pos.x = p1->x;
		//	vertices[2].pos.y = p1->y;
		//	vertices[2].pos.z = 0.f;

		//	vertices[3].pos.x = p2->x;
		//	vertices[3].pos.y = p2->y;
		//	vertices[3].pos.z = 0.f;


		//	vertices[4].pos.x = p2->x;
		//	vertices[4].pos.y = p2->y;
		//	vertices[4].pos.z = 0.f;

		//	vertices[5].pos.x = p0->x;
		//	vertices[5].pos.y = p0->y;
		//	vertices[5].pos.z = 0.f;

		//	for( size_t i = 0; i != 6; ++i )
		//	{
		//		vertices[i].color = 0xffffffff;
		//		vertices[i].uv.x = 0.f;
		//		vertices[i].uv.y = 0.f;
		//		vertices[i].uv2.x = 0.f;
		//		vertices[i].uv2.y = 0.f;
		//	}

		//	const RenderMaterial * material = RENDER_SERVICE(m_serviceProvider)
		//		->getDebugMaterial();

		//	const RenderViewportInterface * renderViewport = PLAYER_SERVICE(m_serviceProvider)
		//		->getRenderViewport();

		//	const RenderCameraInterface * renderCamera = PLAYER_SERVICE(m_serviceProvider)
		//		->getRenderCamera();

		//	RENDER_SERVICE(m_serviceProvider)
		//		->addRenderLine( renderViewport, renderCamera, material, nullptr, 0, vertices, 6 );
		//}

		//const Poly2Tri::TListTriangle & map = m_cdt->GetMap();
	}
}
#	include "PathFinderWay.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	PathFinderWay::PathFinderWay()
		: m_serviceProvider(nullptr)
		, m_from(0.f, 0.f)
		, m_to(0.f, 0.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	PathFinderWay::~PathFinderWay()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void PathFinderWay::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	void PathFinderWay::initialize( const mt::vec2f & _from, const mt::vec2f & _to, size_t _capacity )
	{
		m_from = _from;
		m_to = _to;

		m_way.reserve( _capacity );
	}
	//////////////////////////////////////////////////////////////////////////
	void PathFinderWay::addPoint( const mt::vec2f & _point )
	{
		m_way.push_back( _point );
	}
	//////////////////////////////////////////////////////////////////////////
	void PathFinderWay::render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera )
	{
		size_t numpoints = m_way.size();

		if( numpoints == 0 )
		{
			return;
		}

		size_t vertexCount = numpoints * 2;

		RenderVertex2D * vertices = RENDER_SERVICE(m_serviceProvider)
			->getDebugRenderVertex2D( vertexCount );

		if( vertices == nullptr )
		{
			return;
		}

		for( size_t i = 0; i != (numpoints-1); ++i )
		{
			size_t j = (i + 1) % numpoints;

			const mt::vec2f & trP0 = m_way[i];
			RenderVertex2D & v0 = vertices[i*2+0];

			v0.pos.x = trP0.x;
			v0.pos.y = trP0.y;
			v0.pos.z = 0.f;

			v0.color = 0xFF00FF00;
			v0.uv.x = 0.f;
			v0.uv.y = 0.f;
			v0.uv2.x = 0.f;
			v0.uv2.y = 0.f;

			const mt::vec2f & trP1 = m_way[j];
			RenderVertex2D & v1 = vertices[i*2+1];

			v1.pos.x = trP1.x;
			v1.pos.y = trP1.y;
			v1.pos.z = 0.f;

			v1.color = 0xFF00FF00;
			v1.uv.x = 0.f;
			v1.uv.y = 0.f;
			v1.uv2.x = 0.f;
			v1.uv2.y = 0.f;
		}

		const RenderMaterialInterfacePtr & debugMaterial = RENDER_SERVICE(m_serviceProvider)
			->getDebugMaterial();

		RENDER_SERVICE(m_serviceProvider)->addRenderLine( _viewport, _camera, debugMaterial
			, vertices
			, vertexCount
			, nullptr
			);
	}
}
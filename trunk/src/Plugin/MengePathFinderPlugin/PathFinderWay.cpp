#	include "PathFinderWay.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	PathFinderWay::PathFinderWay( ServiceProviderInterface * _serviceProvider )
		: m_serviceProvider(_serviceProvider)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	PathFinderWay::~PathFinderWay()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void PathFinderWay::initialize( const mt::vec2f & _from, const mt::vec2f & _to, const TVectorPathPoint & _path )
	{
		size_t pathCount = _path.size();

		m_wayPoint.resize( pathCount );

		for( size_t i = 0; i != pathCount; ++i )
		{
			Poly2Tri::Point * point = _path[i];
			m_wayPoint[i] = mt::vec2f(point->x, point->y);
		}

		m_from = _from;
		m_to = _to;
	}
	//////////////////////////////////////////////////////////////////////////
	void PathFinderWay::render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera )
	{
		size_t numpoints = m_wayPoint.size();

		if( numpoints == 0 )
		{
			return;
		}

		size_t vertexCount = (numpoints + 2) * 2;

		RenderVertex2D * vertices = RENDER_SERVICE(m_serviceProvider)
			->getDebugRenderVertex2D( vertexCount );

		if( vertices == nullptr )
		{
			return;
		}

		{
			mt::vec2f trP0 = m_from;
			RenderVertex2D & v0 = vertices[0];

			v0.pos.x = trP0.x;
			v0.pos.y = trP0.y;
			v0.pos.z = 0.f;

			v0.color = 0xFF00FF00;
			v0.uv.x = 0.f;
			v0.uv.y = 0.f;
			v0.uv2.x = 0.f;
			v0.uv2.y = 0.f;

			mt::vec2f trP1 = m_wayPoint.front();
			RenderVertex2D & v1 = vertices[1];

			v1.pos.x = trP1.x;
			v1.pos.y = trP1.y;
			v1.pos.z = 0.f;

			v1.color = 0xFF00FF00;
			v1.uv.x = 0.f;
			v1.uv.y = 0.f;
			v1.uv2.x = 0.f;
			v1.uv2.y = 0.f;
		}

		{
			mt::vec2f trP0 = m_wayPoint.back();
			RenderVertex2D & v0 = vertices[(numpoints + 1) * 2 + 0];

			v0.pos.x = trP0.x;
			v0.pos.y = trP0.y;
			v0.pos.z = 0.f;

			v0.color = 0xFF00FF00;
			v0.uv.x = 0.f;
			v0.uv.y = 0.f;
			v0.uv2.x = 0.f;
			v0.uv2.y = 0.f;

			mt::vec2f trP1 = m_to;
			RenderVertex2D & v1 = vertices[(numpoints + 1) * 2 + 1];

			v1.pos.x = trP1.x;
			v1.pos.y = trP1.y;
			v1.pos.z = 0.f;

			v1.color = 0xFF00FF00;
			v1.uv.x = 0.f;
			v1.uv.y = 0.f;
			v1.uv2.x = 0.f;
			v1.uv2.y = 0.f;
		}

		for( size_t i = 0; i != (numpoints - 1); ++i )
		{
			size_t j = (i + 1) % numpoints;

			mt::vec2f trP0 = m_wayPoint[i];
			RenderVertex2D & v0 = vertices[(i+1)*2+0];

			v0.pos.x = trP0.x;
			v0.pos.y = trP0.y;
			v0.pos.z = 0.f;

			v0.color = 0xFF00FF00;
			v0.uv.x = 0.f;
			v0.uv.y = 0.f;
			v0.uv2.x = 0.f;
			v0.uv2.y = 0.f;

			mt::vec2f trP1 = m_wayPoint[j];
			RenderVertex2D & v1 = vertices[(i+1)*2+1];

			v1.pos.x = trP1.x;
			v1.pos.y = trP1.y;
			v1.pos.z = 0.f;

			v1.color = 0xFF00FF00;
			v1.uv.x = 0.f;
			v1.uv.y = 0.f;
			v1.uv2.x = 0.f;
			v1.uv2.y = 0.f;
		}

		const RenderMaterial * debugMaterial = RENDER_SERVICE(m_serviceProvider)
			->getDebugMaterial();

		RENDER_SERVICE(m_serviceProvider)->addRenderLine( _viewport, _camera, debugMaterial, nullptr, 0
			, vertices
			, vertexCount
			);
	}
}
#	include "Point.h"

#   include "Interface/RenderSystemInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Point::Point()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Point::_debugRender( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, unsigned int _debugMask )
	{
		(void)_viewport;
        (void)_camera;
        (void)_debugMask;

		if( (_debugMask & MENGE_DEBUG_HOTSPOTS) == 0 )
		{
			return;
		}

		const mt::vec3f & pos = this->getWorldPosition();

		RenderVertex2D * vertexPointBox = RENDER_SERVICE(m_serviceProvider)
			->getDebugRenderVertex2D( 4 * 2 );

		float debugWidth = 5.f;

		vertexPointBox[0].pos.x = pos.x - debugWidth;
		vertexPointBox[0].pos.y = pos.y - debugWidth;

		vertexPointBox[1].pos.x = pos.x + debugWidth;
		vertexPointBox[1].pos.y = pos.y - debugWidth;

		vertexPointBox[2].pos.x = pos.x + debugWidth;
		vertexPointBox[2].pos.y = pos.y - debugWidth;

		vertexPointBox[3].pos.x = pos.x + debugWidth;
		vertexPointBox[3].pos.y = pos.y + debugWidth;

		vertexPointBox[4].pos.x = pos.x + debugWidth;
		vertexPointBox[4].pos.y = pos.y + debugWidth;

		vertexPointBox[5].pos.x = pos.x - debugWidth;
		vertexPointBox[5].pos.y = pos.y + debugWidth;

		vertexPointBox[6].pos.x = pos.x - debugWidth;
		vertexPointBox[6].pos.y = pos.y + debugWidth;

		vertexPointBox[7].pos.x = pos.x - debugWidth;
		vertexPointBox[7].pos.y = pos.y - debugWidth;


		for( uint32_t i = 0; i != 8; ++i )
		{
			vertexPointBox[i].pos.z = 0.f;

			vertexPointBox[i].color = 0xFF00FF00;
			vertexPointBox[i].uv.x = 0.f;
			vertexPointBox[i].uv.y = 0.f;
			vertexPointBox[i].uv2.x = 0.f;
			vertexPointBox[i].uv2.y = 0.f;
		}

		const RenderMaterialInterfacePtr & debugMaterial = RENDER_SERVICE(m_serviceProvider)
			->getDebugMaterial();

		RENDER_SERVICE(m_serviceProvider)
			->addRenderLine( _viewport, _camera, debugMaterial, vertexPointBox, 8, nullptr );
	}
	//////////////////////////////////////////////////////////////////////////

}

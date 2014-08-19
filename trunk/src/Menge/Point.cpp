#	include "Point.h"

#   include "Interface/RenderSystemInterface.h"

namespace	Menge
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
		//const mt::vec3f& pos = getWorldPosition();

		//mt::vec2f pos;
		//mt::mul_v2_m3_r( pos, getWorldPosition(), getWorldMatrix() );
		//mt::vec3f pos1( pos + mt::vec2f( -5.0f, -5.0f ) );
		//mt::vec3f pos2( pos + mt::vec2f( 5.0f, -5.0f ) );
		//mt::vec3f pos3( pos + mt::vec2f( 5.0f, 5.0f ) );
		//mt::vec3f pos4( pos + mt::vec2f( -5.0f, 5.0f ) );
		//renderEngine->renderLine( 0xFF00FFFF, pos1, pos2 );
		//renderEngine->renderLine( 0xFF00FFFF, pos2, pos3 );
		//renderEngine->renderLine( 0xFF00FFFF, pos1, pos4 );
		//renderEngine->renderLine( 0xFF00FFFF, pos4, pos3 );
	}
	//////////////////////////////////////////////////////////////////////////

}

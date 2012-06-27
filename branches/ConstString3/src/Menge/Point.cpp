#	include "Point.h"

#	include "Camera2D.h"
#	include "HotSpot.h"

#	include "RenderEngine.h"
#	include "Sprite.h"

#	include "Application.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	Point::Point()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool Point::testHotSpot( RenderCameraInterface * _camera2D, HotSpot * _hotspot )
	{
		mt::vec2f pos = this->getCameraPosition( _camera2D );

		bool result = _hotspot->testPoint( pos );
		return result;
	}
//#	ifndef MENGE_MASTER_RELEASE
	//////////////////////////////////////////////////////////////////////////
	void Point::_debugRender( RenderCameraInterface * _camera, unsigned int _debugMask )
	{
		if( _debugMask & MENGE_DEBUG_HOTSPOTS )
		{
			const mt::vec3f& pos = getWorldPosition();

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
	}
//#	endif
	//////////////////////////////////////////////////////////////////////////

}

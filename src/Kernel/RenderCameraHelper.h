#	pragma once

#	include "Interface/RenderSystemInterface.h"

namespace Menge
{
	namespace Helper
	{
		//////////////////////////////////////////////////////////////////////////
		void screenToWorldPosition( const RenderCameraInterface * _renderCamera, const mt::vec2f & _screenPoint, mt::vec2f & _worldPoint );
		void screenToWorldDelta( const RenderCameraInterface * _renderCamera, const mt::vec2f & _screenPoint, const mt::vec2f & _screenDeltha, mt::vec2f & _worldDeltha );
        void worldToScreenBox( const RenderCameraInterface * _renderCamera, const RenderViewportInterface * _renderViewport, const Resolution & _contentResolution, const mt::box2f & _worldBox, mt::box2f & _screenBox );
	}
}

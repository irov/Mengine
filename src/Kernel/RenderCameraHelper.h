#pragma once

#include "Interface/RenderSystemInterface.h"

namespace Mengine
{
	namespace Helper
	{
		//////////////////////////////////////////////////////////////////////////
		void screenToWorldPosition( const RenderCameraInterfacePtr & _renderCamera, const mt::vec2f & _screenPoint, mt::vec2f & _worldPoint );
		void screenToWorldDelta( const RenderCameraInterfacePtr & _renderCamera, const mt::vec2f & _screenPoint, const mt::vec2f & _screenDeltha, mt::vec2f & _worldDeltha );
        void worldToScreenBox( const RenderCameraInterfacePtr & _renderCamera, const RenderViewportInterfacePtr & _renderViewport, const Resolution & _contentResolution, const mt::box2f & _worldBox, mt::box2f & _screenBox );
	}
}

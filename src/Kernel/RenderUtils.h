#pragma once

#include "Interface/RenderImageInterface.h"

#include "Kernel/RenderVertex2D.h"
#include "Kernel/ColourValue.h"

#include "math/box2.h"

namespace Mengine
{
	namespace Helper
	{
		//////////////////////////////////////////////////////////////////////////
        void makeRenderBoundingBox( mt::box2f & _bb, const RenderVertex2D * _vertices, uint32_t _verticesCount );
        //////////////////////////////////////////////////////////////////////////
        uint32_t getImageMemorySize( const RenderImageInterfacePtr & _image );
        //////////////////////////////////////////////////////////////////////////
        void multiplyColorBuffer( const ColourValue & _color, uint8_t * _out, const uint8_t * _in );
        //////////////////////////////////////////////////////////////////////////
	}
}
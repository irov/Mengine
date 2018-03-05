#	pragma once

#	include "Math/box2.h"

namespace Menge
{
	namespace Helper
	{
		//////////////////////////////////////////////////////////////////////////
		inline void makeRenderBoundingBox( mt::box2f & _bb, const RenderVertex2D * _vertices, uint32_t _verticesCount )
		{
			if( _verticesCount == 0 )
			{
				mt::reset( _bb, 0.f, 0.f );
				return;
			}

			const RenderVertex2D & v0 = _vertices[0];
			mt::reset( _bb, v0.position.x, v0.position.y );

			for( uint32_t i = 1; i != _verticesCount; ++i )
			{
				const RenderVertex2D & v = _vertices[i];

				mt::add_internal_point( _bb, v.position.x, v.position.y );
			}
		}
        //////////////////////////////////////////////////////////////////////////
        inline size_t getImageMemorySize( const RenderImageInterfacePtr & _image )
        {
            uint32_t HWWidth = _image->getHWWidth();
            uint32_t HWHeight = _image->getHWHeight();
            uint32_t HWChannels = _image->getHWChannels();
            uint32_t HWDepth = _image->getHWDepth();
            PixelFormat HWPixelFormat = _image->getHWPixelFormat();

            size_t memorySize = Helper::getTextureMemorySize( HWWidth, HWHeight, HWChannels, HWDepth, HWPixelFormat );

            return memorySize;
        }
	}
}
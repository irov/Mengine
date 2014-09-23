#	pragma once

#	include "Interface/RenderSystemInterface.h"

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
			mt::reset( _bb, v0.pos.x, v0.pos.y );

			for( uint32_t i = 1; i != _verticesCount; ++i )
			{
				const RenderVertex2D & v = _vertices[i];

				mt::add_internal_point( _bb, v.pos.x, v.pos.y );
			}
		}
	}
}
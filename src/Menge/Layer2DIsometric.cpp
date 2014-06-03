#	include "Layer2DIsometric.h"

#	include "Kernel/Scene.h"

#	include "Camera2D.h"

#	include "Interface/NodeInterface.h"
#   include "Interface/RenderSystemInterface.h"

#	include "Logger/Logger.h"

#	include "Consts.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	Layer2DIsometric::Layer2DIsometric()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		class FIsometricSortY
		{
		public:
			bool operator() ( Node * _left, Node * _right ) const
			{
				const mt::vec3f & l = _left->getLocalPosition();
				const mt::vec3f & r = _right->getLocalPosition();

				float less_y =  l.y - r.y;
				if( less_y < 0.1f && less_y > -0.1f )
				{
					return l.x < r.x;
				}

				return l.y < r.y;
			}
		};		
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2DIsometric::render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, unsigned int _debugMask )
	{
		stdex::intrusive_sort_stable( m_child, FIsometricSortY() );

		Layer::render( _viewport, _camera, _debugMask );
	}
}

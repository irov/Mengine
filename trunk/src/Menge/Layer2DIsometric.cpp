#	include "Layer2DIsometric.h"

#	include "Kernel/Scene.h"

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

				const mt::vec3f & l_offset = _left->getIsometricOffset();
				const mt::vec3f & r_offset = _right->getIsometricOffset();

				float l_y = l.y + l_offset.y;
				float r_y = r.y + r_offset.y;

				float less_y = l_y - r_y;
				if( less_y < 0.1f && less_y > -0.1f )
				{
					float l_x = l.x + l_offset.x;
					float r_x = r.x + r_offset.x;

					return l_x < r_x;
				}

				return l_y < r_y;
			}
		};
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2DIsometric::render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, unsigned int _debugMask )
	{
		stdex::intrusive_sort_stable( m_children, FIsometricSortY() );

		Layer::render( _viewport, _camera, _debugMask );
	}
}

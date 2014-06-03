#	include "Isometric.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	Isometric::Isometric()
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
	void Isometric::render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, unsigned int _debugMask )
	{
		stdex::intrusive_sort_stable( m_child, FIsometricSortY() );

		Node::render( _viewport, _camera, _debugMask );
	}
}

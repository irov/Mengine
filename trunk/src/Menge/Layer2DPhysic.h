#	pragma once

#	include "Layer2D.h"

namespace Menge
{
	class Camera2D;

	class Layer2DPhysic
		: public Layer2D
	{
        DECLARE_VISITABLE();

	public:
		Layer2DPhysic();

	protected:
		bool _compile() override;
		void _release() override;
		void _update( float _current, float _timing ) override;
		void _removeChildren( Node * _node ) override;
	};
}

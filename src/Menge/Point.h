#	pragma once

#	include "Kernel/Node.h"

namespace Menge
{
	class Point
		: public Node
	{
	public:
		Point();

	protected:
		void _debugRender( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, unsigned int _debugMask ) override;
	};
}

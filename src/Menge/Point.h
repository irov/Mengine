#	pragma once

#	include "Kernel/Node.h"

namespace Menge
{
	class HotSpot;

	class Point
		: public Node
	{
	public:
		Point();

	//public:
		//bool testHotSpot( RenderCameraInterface * _camera, HotSpot * _hotspot );

	protected:
		void _debugRender( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, unsigned int _debugMask ) override;
	};
}

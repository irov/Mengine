#	pragma once

#	include "Node.h"

namespace Menge
{
	class HotSpot;

	class Point
		: public Node
	{
		FACTORABLE_DECLARE(Point)

	public:
		Point();

	public:
		bool testHotSpot( HotSpot * _hotspot );

#	ifndef MENGE_MASTER_RELEASE
	protected:
		void _debugRender( Camera2D * _camera, unsigned int _debugMask ) override;
#	endif
	};
}

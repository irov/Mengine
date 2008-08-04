#	pragma once

#	include "Node.h"

namespace Menge
{
	class HotSpot;

	class Point
		: public Node
	{
		OBJECT_DECLARE(Point)

	public:
		Point();

	public:
		bool testHotSpot( HotSpot * _hotspot );

	public:

		void _render( bool _enableDebug ) override;
	};
}
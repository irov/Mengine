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
		void loader( XmlElement * _xml ) override;

		void _render( const Viewport & _viewport, bool _enableDebug ) override;
	protected:
		void _update( float _timing ) override;
		bool _activate() override;
	};
}
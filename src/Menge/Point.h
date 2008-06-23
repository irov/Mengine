#	pragma once

#	include "SceneNode2D.h"

namespace Menge
{
	class HotSpot;

	class Point
		: public SceneNode2D
	{
		OBJECT_DECLARE(Point)

	public:
		Point();

	public:
		bool testHotSpot( HotSpot * _hotspot );

	public:
		void loader( XmlElement * _xml ) override;

	protected:
		void _update( float _timing ) override;
		bool _activate() override;
		void _render( bool _enableDebug ) override;
		void _debugRender() override;
	};
}
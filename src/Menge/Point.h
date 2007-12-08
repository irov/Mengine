#	pragma once

#	include "SceneNode2D.h"

#	include "MousePickerTrap.h"

#	include "InputHandler.h"

namespace Menge
{
	class HotSpot;

	class Point
		: public SceneNode2D
		, public MousePickerTrap
		, public InputHandler
	{
		OBJECT_DECLARE(Point)

	public:
		Point();

	public:
		bool testHotSpot( HotSpot * _hotspot );
		bool pick( const mt::vec2f& _offset, HotSpot * _hotspot ) override;

		void onLeave() override;
		void onEnter() override;

	public:
		bool handleKeyEvent( size_t _key, bool _isDown ) override;
		bool handleMouseButtonEvent( size_t _button, bool _isDown ) override;
		bool handleMouseMove( int _x, int _y, int _whell ) override;

	public:
		void loader( TiXmlElement * _xml ) override;

	protected:
		void _update( float _timing ) override;
		bool _activate() override;
		void _debugRender() override;

	private:
		mt::vec2f m_offset;
	};
}
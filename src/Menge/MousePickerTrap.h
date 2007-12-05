#	pragma once

#	include "Node.h"
#	include "Math/vec2.h"

namespace Menge
{
	class HotSpot;
	class InputHandler;

	class MousePickerTrap
		: public virtual Node
	{
	public:
		MousePickerTrap();

	public:
		void setHandler( InputHandler * _handler );
		InputHandler * handler();

	public:
		virtual bool pick( const mt::vec2f & _offset, HotSpot * _hotspot ) = 0;
		virtual void onLeave() = 0;
		virtual void onEnter() = 0;

	private:
		InputHandler * m_handler;
	};
}
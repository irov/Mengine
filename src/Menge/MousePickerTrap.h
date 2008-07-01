#	pragma once

#	include "Node.h"
#	include "Math/vec2.h"

namespace Menge
{
	class HotSpot;
	class InputHandler;

	class MousePickerTrap
	//	: public virtual Node
	{
	public:
		MousePickerTrap();

	public:
		void setHandler( InputHandler * _handler );
		InputHandler * handler();

	public:
		virtual bool pick( HotSpot * _hotspot ) = 0;
		virtual void onLeave() = 0;
		virtual void onEnter() = 0;

	public:
		virtual bool _pickerActive() = 0;

	private:
		InputHandler * m_handler;
	};
}
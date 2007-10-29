#	pragma once

#	include "Node.h"

namespace Menge
{
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
		virtual bool pick( float _x, float _y ) = 0;
		virtual void onLeave() = 0;
		virtual void onEnter() = 0;

	private:
		InputHandler * m_handler;
	};
}
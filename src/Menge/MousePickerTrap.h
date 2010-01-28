#	pragma once

#	include "Math/vec2.h"

namespace Menge
{
	class Arrow;
	class InputHandler;

	class MousePickerTrap
	{
	public:
		MousePickerTrap();

	public:
		void setHandler( InputHandler * _handler );
		InputHandler * handler();

	public:
		virtual bool pick( Arrow * _arrow ) = 0;

	public:
		virtual bool onEnter() = 0;
		virtual void onLeave() = 0;

	public:
		virtual bool _pickerActive() const = 0;

	private:
		InputHandler * m_handler;
	};
}
